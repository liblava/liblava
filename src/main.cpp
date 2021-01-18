// file      : src/main.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

int main(int argc, char* argv[]) {
    app app("template", { argc, argv });
    if (!app.setup())
        return error::not_ready;

    return app.run();
}
