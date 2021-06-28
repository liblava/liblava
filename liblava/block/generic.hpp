//-----------------------------------------------------------------------------
#define __build_field(__struct, __field) = __struct.__field

// TODO: With complex macros, it is possible to make vertex_T default to lava::vertex via optional arguments.
// Or maybe that should just be handled the same way as generic_mesh<T>::move and ::scale instead.
#define generic_create_mesh(device, type, input_mesh, vertex_T, vertex_position_field) \
    /* Provide error messages on misuse. */ \
    static_assert(std::is_same<lava::device_ptr, device>, "device argument must be type lava::device_ptr"); \
    static_assert(type == lava::mesh_type::triangle, "Only triangle is supported currently."); \
    /* If the vertex layout is default, simply fall into create_mesh() */ \
    if constexpr (std::is_same_v<vertex_T, lava::vertex>) { \
        input_mesh->create_mesh(device, type); \
    } else { \
        using ptr_t = std::shared_ptr<generic_mesh<vertex_T>>; \
        switch (type) { \
        case lava::mesh_type::triangle: { \
            ptr_t cube = generic_make_mesh<vertex_T>(); \
            auto vert_one = vertexT; \
            auto vert_two = vertexT; \
            auto vert_three = vertexT; \
            __build_field(vert_one, vertex_position_field) = { 1, 1, 0 }; \
            __build_field(vert_two, vertex_position_field) = { -1, 1, 0 }; \
            __build_field(vert_three, vertex_position_field) = { 0, -1, 0 }; \
        } \
        } \
    }
