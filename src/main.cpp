// file      : src-demo/main.cpp
// authors   : Lava Block OÜ and contributors
// copyright : Copyright (c) 2018-present, MIT License

#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    app app("template", { argc, argv });
    if (!app.setup())
        return error::not_ready;

    return app.run();
}
