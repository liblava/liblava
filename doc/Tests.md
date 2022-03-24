<a href="https://git.io/liblava"><img align="left" src="../res/doc/liblava_logo_200px.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.7.1-blue)](https://git.io/liblava)

[Home](README.md) &nbsp; [Features](Features.md) &nbsp; [Tutorial](Tutorial.md) &nbsp; [Guide](Guide.md) &nbsp; [Reference](Reference.md) &nbsp; [Modules](Modules.md) &nbsp; [Third-Party](Third-Party.md) &nbsp; [Demo](Demo.md) &nbsp; **[Tests](Tests.md)** &nbsp; [Install](Install.md)

<br />

# Tests

Run the `lava` executable to test our [Tutorial](../tests/tests.cpp) examples

<br />

## List all tests

```bash
lava -ts

lava --tests
```

1. [frame init](Tutorial.md/#1-frame-init)
2. [run loop](Tutorial.md/#2-run-loop)
3. [window input](Tutorial.md/#3-window-input)
4. [clear color](Tutorial.md/#4-clear-color)
5. [color block](Tutorial.md/#5-color-block)
6. forward shading
7. gamepad
8. [imgui demo](Tutorial.md/#8-imgui-demo)

<br />

### Run *window input* test

```bash
lava -t=3

lava --test=3
```


If you run `lava` without arguments - the *last* will be started

<br />

## Unit testing

In addition run `lava-unit` to check some parts of the library with [Catch2](https://github.com/catchorg/Catch2) framework

<br />

<a href="https://git.io/liblava"><img src="../res/doc/liblava_logo_200px.png" width="50"></a>
