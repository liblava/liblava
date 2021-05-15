<a href="https://git.io/liblava"><img align="left" src="https://github.com/liblava.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.6.2-blue)](https://git.io/liblava)

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

<br />

## Command-Line Arguments

### lava app

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
