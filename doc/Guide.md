<a href="https://git.io/liblava"><img align="left" src="https://github.com/liblava.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.6.2-blue)](https://git.io/liblava)

[Home](README.md) &nbsp; [Features](Features.md) &nbsp; [Tutorial](Tutorial.md) &nbsp; **[Guide](Guide.md)** &nbsp; [Modules](Modules.md) &nbsp; [Third-Party](Third-Party.md) &nbsp; [Demo](Demo.md) &nbsp; [Tests](Tests.md) &nbsp; [Install](Install.md)

<br />

# Guide

*WIP*

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

<a href="https://git.io/liblava"><img src="https://github.com/liblava.png" width="50"></a>
