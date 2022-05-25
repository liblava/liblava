/**
 * @file         liblava/engine/producer.cpp
 * @brief        Producer
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/asset.hpp>
#include <liblava/base/instance.hpp>
#include <liblava/engine/engine.hpp>
#include <liblava/engine/producer.hpp>
#include <liblava/file/file_system.hpp>
#include <liblava/file/file_utils.hpp>
#include <shaderc/shaderc.hpp>

namespace lava {

//-----------------------------------------------------------------------------
mesh::ptr producer::create_mesh(mesh_type mesh_type) {
    auto product = lava::create_mesh(context->device, mesh_type);
    if (!add_mesh(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
mesh::ptr producer::get_mesh(string_ref name) {
    for (auto& [id, meta] : meshes.get_all_meta()) {
        if (meta == name)
            return meshes.get(id);
    }

    auto product = load_mesh(context->device, context->prop.get_filename(name));
    if (!product)
        return nullptr;

    if (!add_mesh(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
bool producer::add_mesh(mesh::ptr product) {
    if (!product)
        return false;

    if (meshes.exists(product->get_id()))
        return false;

    meshes.add(product);
    return true;
}

//-----------------------------------------------------------------------------
texture::ptr producer::create_texture(uv2 size) {
    auto product = create_default_texture(context->device, size);
    if (!add_texture(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
texture::ptr producer::get_texture(string_ref name) {
    for (auto& [id, meta] : textures.get_all_meta()) {
        if (meta == name)
            return textures.get(id);
    }

    auto product = load_texture(context->device, context->prop.get_filename(name));
    if (!product)
        return nullptr;

    if (!add_texture(product))
        return nullptr;

    return product;
}

//-----------------------------------------------------------------------------
bool producer::add_texture(texture::ptr product) {
    if (!product)
        return false;

    if (textures.exists(product->get_id()))
        return false;

    textures.add(product);

    context->staging.add(product);
    return true;
}

//-----------------------------------------------------------------------------
cdata producer::get_shader(string_ref name, bool reload) {
    if (shaders.count(name)) {
        if (!reload)
            return shaders.at(name);

        shaders.at(name).free();
        shaders.erase(name);
    }

    string pref_dir = file_system::get_pref_dir();

    string shader_path = "cache/shader/";
    auto path = pref_dir + shader_path + name + ".spirv";

    if (!reload) {
        data module_data;
        if (load_file_data(path, module_data)) {
            shaders.emplace(name, module_data);

            log()->info("shader cache: {} - {} bytes", name, module_data.size);

            return module_data;
        }
    }

    if (reload)
        context->prop.unload(name);

    auto product = context->prop(name);
    if (!product.ptr)
        return {};

    auto module_data = compile_shader(product, name, context->prop.get_filename(name));
    if (!module_data.ptr)
        return {};

    context->prop.unload(name);

    file_system::instance().create_folder(str(shader_path));

    file file(str(path), file_mode::write);
    if (file.opened())
        if (!file.write(module_data.ptr, module_data.size))
            log()->warn("shader not cached: {}", str(path));

    shaders.emplace(name, module_data);

    return module_data;
}

/**
 * @brief Shader includer
 */
class shader_includer : public shaderc::CompileOptions::IncluderInterface {
public:
    /**
     * @brief Construct a new shader includer
     *
     * @param path    Current file path
     */
    shader_includer(std::filesystem::path path)
    : path(path) {
    }

    /**
     * @brief Handle include result data
     *
     * @param requested_source            Requested source
     * @param type                        Include type
     * @param requesting_source           Requesting source
     * @param include_depth               Include depth
     *
     * @return shaderc_include_result*    Include result
     */
    shaderc_include_result* GetInclude(
        char const* requested_source,
        shaderc_include_type type,
        char const* requesting_source,
        size_t include_depth) override {
        string const name = string(requested_source);

        auto file_path = path;
        file_path.replace_filename(name);

        file_data file_data(file_path.lexically_normal().string());
        if (!file_data.ptr)
            return nullptr;

        auto container = new std::array<std::string, 2>;
        (*container)[0] = name;
        (*container)[1] = { file_data.ptr, file_data.size };

        auto data = new shaderc_include_result;

        data->user_data = container;

        data->source_name = (*container)[0].data();
        data->source_name_length = (*container)[0].size();

        data->content = (*container)[1].data();
        data->content_length = (*container)[1].size();

        return data;
    };

    /**
     * @brief Handle include result release
     *
     * @param data    Include result
     */
    void ReleaseInclude(shaderc_include_result* data) override {
        delete static_cast<std::array<std::string, 2>*>(data->user_data);
        delete data;
    };

private:
    /// Current file path
    std::filesystem::path path;
};

/**
 * @brief Get shader kind by file extension
 *
 * @param filename                Name of file
 *
 * @return shaderc_shader_kind    Shader kind
 */
shaderc_shader_kind get_shader_kind(string_ref filename) {
    if (extension(str(filename), "vert"))
        return shaderc_glsl_vertex_shader;
    if (extension(str(filename), "frag"))
        return shaderc_glsl_fragment_shader;
    if (extension(str(filename), "comp"))
        return shaderc_compute_shader;
    if (extension(str(filename), "geom"))
        return shaderc_geometry_shader;
    if (extension(str(filename), "tesc"))
        return shaderc_tess_control_shader;
    if (extension(str(filename), "tese"))
        return shaderc_tess_evaluation_shader;
    if (extension(str(filename), "rgen"))
        return shaderc_raygen_shader;
    if (extension(str(filename), "rint"))
        return shaderc_intersection_shader;
    if (extension(str(filename), "rahit"))
        return shaderc_anyhit_shader;
    if (extension(str(filename), "rchit"))
        return shaderc_closesthit_shader;
    if (extension(str(filename), "rmiss"))
        return shaderc_miss_shader;
    if (extension(str(filename), "rcall"))
        return shaderc_callable_shader;

    return shaderc_glsl_infer_from_source;
}

//-----------------------------------------------------------------------------
data producer::compile_shader(cdata product, string_ref name, string_ref filename) const {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetIncluder(std::make_unique<shader_includer>(filename));

    auto shader_type = get_shader_kind(filename);

    auto opt_level = shaderc_optimization_level_zero;
    if (shader_opt == producer::shader_optimization::size)
        opt_level = shaderc_optimization_level_size;
    else if (shader_opt == producer::shader_optimization::performance)
        opt_level = shaderc_optimization_level_performance;

    options.SetOptimizationLevel(opt_level);

    auto source_lang = shaderc_source_language::shaderc_source_language_glsl;
    if (shader_lang == producer::shader_language::hlsl)
        source_lang = shaderc_source_language::shaderc_source_language_hlsl;

    options.SetSourceLanguage(source_lang);

    if (shader_debug)
        options.SetGenerateDebugInfo();

    switch (instance::singleton().get_info().req_api_version) {
    case api_version::v1_1:
        options.SetTargetEnvironment(shaderc_target_env_default, shaderc_env_version_vulkan_1_1);
        options.SetTargetSpirv(shaderc_spirv_version_1_3);
        break;
    case api_version::v1_2:
        options.SetTargetEnvironment(shaderc_target_env_default, shaderc_env_version_vulkan_1_2);
        options.SetTargetSpirv(shaderc_spirv_version_1_5);
        break;
    case api_version::v1_3:
        options.SetTargetEnvironment(shaderc_target_env_default, shaderc_env_version_vulkan_1_3);
        options.SetTargetSpirv(shaderc_spirv_version_1_6);
        break;
    default:
        options.SetTargetEnvironment(shaderc_target_env_default, shaderc_env_version_vulkan_1_0);
        options.SetTargetSpirv(shaderc_spirv_version_1_0);
    }

    shaderc::PreprocessedSourceCompilationResult result =
        compiler.PreprocessGlsl({ product.ptr, product.size }, shader_type, str(name), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        log()->error("preprocess shader: {} - {}", name, result.GetErrorMessage());
        return {};
    }

    shaderc::SpvCompilationResult module =
        compiler.CompileGlslToSpv({ result.cbegin(), result.cend() }, shader_type, str(name), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        log()->error("compile shader: {} - {}", name, module.GetErrorMessage());
        return {};
    }

    std::vector<ui32> const module_result = { module.cbegin(), module.cend() };

    auto data_size = module_result.size() * sizeof(ui32);
    log()->info("shader compiled: {} - {} bytes", name, data_size);

    data module_data;
    module_data.set(data_size);
    memcpy(module_data.ptr, module_result.data(), data_size);

    return module_data;
}

//-----------------------------------------------------------------------------
void producer::destroy() {
    for (auto& [id, mesh] : meshes.get_all())
        mesh->destroy();

    for (auto& [id, texture] : textures.get_all())
        texture->destroy();

    for (auto& [prop, shader] : shaders)
        shader.free();
}

//-----------------------------------------------------------------------------
void producer::clear() {
    destroy();

    meshes.clear();
    textures.clear();
    shaders.clear();
}

} // namespace lava
