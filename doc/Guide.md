<a href="https://git.io/liblava"><img align="left" src="https://github.com/liblava.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.7.1-blue)](https://git.io/liblava)

[Home](README.md) &nbsp; [Features](Features.md) &nbsp; [Tutorial](Tutorial.md) &nbsp; **[Guide](Guide.md)** &nbsp; [Reference](Reference.md) &nbsp; [Modules](Modules.md) &nbsp; [Third-Party](Third-Party.md) &nbsp; [Demo](Demo.md) &nbsp; [Tests](Tests.md) &nbsp; [Install](Install.md)

<br />

# Guide

[Lifetime of an Object](#lifetime-of-an-object) &nbsp; &nbsp; [Making meshes](#making-meshes) &nbsp; &nbsp; [Keyboard shortcuts](#keyboard-shortcuts) &nbsp; &nbsp; [Command-Line Arguments](#command-line-arguments)

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

----

<br />

## Making meshes

**liblava** provides a `mesh` struct that contains a list of vertices and, optionally,
a list of indices

By default, vertices in a `mesh` are of type `vertex`, which has the following layout:

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

<br />

**liblava** provides a `create_mesh()` function to simplify the creation of primitives

It takes a `mesh_type` argument to specify what kind of primitive to build

Its values are: &nbsp; `none` &nbsp; `cube` &nbsp; `triangle` &nbsp; `quad` &nbsp; `hexagon`

The function is called in this way:

```c++
mesh::ptr cube;
cube = create_mesh(device, mesh_type::cube);
```

<br />

Meshes are templated, and can represent any vertex struct definition

```c++
struct int_vertex {
    std::array<i32, 3> position;
    v4 color;
};
mesh_template<int_vertex>::ptr int_triangle;
```

`create_mesh()` can generate primitives for arbitrary vertex structs too,
provided that the struct contains an array or vector member named `position`

```c++
int_triangle = create_mesh<int_vertex>(device, mesh_type::triangle);
```

<br />

`create_mesh()` may also initialize Color, Normal, and UV data automatically

However, it will only initialize these if there are corresponding `color`,
`normal`, and/or `uv` fields defined in the vertex struct

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

// Generate three vertices with positions and uvs, but not colors or normals
triangle = create_mesh<custom_vertex, false, false, true>
                      (device, mesh_type::triangle);
```

Cubes generated this way have a special case. If they are initialized with normal
data, they will be represented by 24 vertices. Otherwise, only 8 vertices will
be initialized

<br />

## Keyboard shortcuts

`lava app` defines some useful shortcuts

| Shortcut             |               Action |   Default   | config.json |  window.json |
| :------------------- | -------------------: | :---------: | ----------: | -----------: |
| *alt + enter*        |  fullscreen on / off |   **off**   |             |   fullscreen |
| *alt + backspace*    |      v-sync on / off |   **off**   |      v-sync |              |
| *control + tab*      |         gui on / off |   **on**    |       imgui |              |
| *control + space*    |       pause on / off |   **off**   |      paused |              |
| *control + q*        |     quit application |             |             |              |

You can disable these actions by simply turning them off:

`app.config.handle_key_events = false`

----

<br />

## Command-Line Arguments

### lava app

```
--clean, -c
```

* clean preferences folder

<br />

```
--v_sync={0|1}, -vs={0|1}
```

* **0** = vertical sync off
* **1** = vertical sync on

<br />

```
--physical_device={n}, -pd={n}
```

* **n** = physical device index | *default: n = 0*

<br />

```
--identification={n}, -id={n}
```

* **n** = window save name | *only numbers*

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
--log={0|...|6}, -l={0|...|6}
```

* level **0** = trace
* level **1** = debug
* level **2** = info
* level **3** = warn
* level **4** = error
* level **5** = critical
* level **6** = logging *off*

----

<br />

*WIP*

<br />

<a href="https://git.io/liblava"><img src="https://github.com/liblava.png" width="50"></a>
