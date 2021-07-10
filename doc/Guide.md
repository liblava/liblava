<a href="https://git.io/liblava"><img align="left" src="https://github.com/liblava.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.6.3-blue)](https://git.io/liblava)

[Home](README.md) &nbsp; [Features](Features.md) &nbsp; [Tutorial](Tutorial.md) &nbsp; **[Guide](Guide.md)** &nbsp; [Reference](Reference.md) &nbsp; [Modules](Modules.md) &nbsp; [Third-Party](Third-Party.md) &nbsp; [Demo](Demo.md) &nbsp; [Tests](Tests.md) &nbsp; [Install](Install.md)

<br />

# Guide

[Lifetime of an Object](#lifetime-of-an-object) / [Command-Line Arguments](#command-line-arguments)

---

<br />

## Lifetime of an Object

Before you create new objects or use existing ones, you should get familiar with the lifetime of objects

It is basically possible to create all objects in **liblava** on the stack or on the heap

But be careful. You have to take care of the lifetime yourself

### make ➜ create ➜ destroy

This is the general pattern that is used in this library:

1. **make** - Use constructor or factory method *(static function to get a shared pointer)*
2. **create** - Build the respective object
3. **destroy** - Discard it after your use

The destructor calls the **destroy** method if it was not called before

### For example: buffer object

```c++
void use_buffer_on_stack() {

    buffer buf; // make

    auto created = buf.create(device, data, size, usage);
    if (created) {
        // ...

        buf.destroy();
    }
}
```

Or look at this method where it is returned as a shared pointer:

```c++
buffer::ptr use_buffer_on_heap() {

    auto buf = make_buffer();

    if (buf->create(device, data, size, usage))
        return buf;

    return nullptr;
}
```

### Making meshes

Liblava provides a `mesh` struct that contains a list of vertices and, optionally,
a list of indices.

By default, vertices in a `mesh` are of type `lava::vertex`, which has the
following layout:

```c++
struct vertex {
    v3 position;
    v4 color;
    v2 uv;
    v3 normal;
}
```

It is made like this:

```c++
mesh::ptr my_mesh = make_mesh();
my_mesh->add_data( /* Pass in a lava::mesh_data object */ );
my_mesh->create(device);
```

Liblava provides a `create_mesh()` function to simplify the creation of primitives. 
It takes a `lava::mesh_type` argument to specify what kind of primitive to build.
Its values are:

```c++
none,
cube,
triangle,
quad
```

The function is called in this way:

```c++
mesh::ptr cube;
cube = create_mesh(device, mesh_type::cube);
```

Meshes are templated, and can represent any vertex struct definition.

```c++
struct int_vertex {
    std::array<int, 3> position;
    v4 color;
};
mesh_template<int_vertex>::ptr int_triangle;
```

`create_mesh()` can generate primitives for arbitrary vertex structs too,
provided that the struct contains an array or vector member named `position`.

```c++
int_triangle = create_mesh<int_vertex>(device, mesh_type::triangle);
```

`create_mesh()` may also initialize color, normal, and UV data automatically.
However, it will only initialize these if there are correspond `color`,
`normal`, and/or `uv` fields defined in the vertex struct.

By default, all data that can be initialized will be, but if generating any 
of this data is not desired, the fields can be individually disabled by template
arguments in this order:
- Color
- Normal
- UV

It is done in this way:

```c++
struct custom_vertex {
    v3 position;
    v3 color;
    v3 normal;
    v2 uv;
};
mesh_template<custom_vertex>::ptr triangle;
// Generate three vertices with position and UVs, but not color or normals.
triangle = create_mesh<custom_vertex, false, false, true>
                      (device, mesh_type::triangle);
```

Cubes generated this way have a special case. If they are initialized with normal
data, they will be represented by 24 vertices. Otherwise, only 8 vertices will
be initialized.

Due to a bug in MSVC, these fields can only be initialized by `create_mesh()` if
the vertex struct is `lava::vertex`. This will be amended when MSVC is more
stable for C++20 compilation.

<br />

## Command-Line Arguments

### lava app

```
--clean, -c
```

* clean preferences folder

<br />

```
--v_sync, -vs {0|1}
```

* 0 ➜ vertical sync off
* 1 ➜ vertical sync on

<br />

```
--physical_device, -pd {n}
```

* n ➜ physical device index

<br />

### lava frame

```
--debug, -d
```

* validation layer ([VK_LAYER_KHRONOS_validation](https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/master/docs/khronos_validation_layer.md))

<br />

```
--utils, -u
```

* debug utils extension ([VK_EXT_debug_utils](https://www.lunarg.com/wp-content/uploads/2018/05/Vulkan-Debug-Utils_05_18_v1.pdf))

<br />

```
--renderdoc, -r
```

* [RenderDoc](https://renderdoc.org/) capture layer (VK_LAYER_RENDERDOC_Capture)

<br />

```
--verbose, -v
```

* verbose debugging

<br />

```
--log, -l {0|1|2|3|4|5|6}
```

* 0 ➜ trace level
* 1 ➜ debug level
* 2 ➜ info level
* 3 ➜ warn level
* 4 ➜ error level
* 5 ➜ critical level
* 6 ➜ logging off

<br />

*WIP*

<br />

<a href="https://git.io/liblava"><img src="https://github.com/liblava.png" width="50"></a>
