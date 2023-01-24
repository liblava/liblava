/**
 * @file         liblava-stage/main.cpp
 * @brief        Main stage driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "imgui.h"
#include "liblava/lava.hpp"

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    auto& driver = driver::instance();

    driver.on_run = [&](argh::parser argh) {
        driver::result result;

        engine app(_lava_, argh);

        if (!app.setup()) {
            result.driver = error::not_ready;
            return result;
        }

        auto const& stages = driver.get_stages();

        app.imgui.on_draw = [&]() {
            ImGui::SetNextWindowPos({ 30, 30 }, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize({ 260, 120 + to_r32(stages.size() * 30) },
                                     ImGuiCond_FirstUseEver);

            ImGui::Begin("stage driver");

            for (auto& [id, stage] : stages) {
                auto stage_label = fmt::format("{}. {}",
                                               id, stage->name);

                if (ImGui::Button(str(stage_label))) {
                    result.selected = id;
                    app.shut_down();
                }
            }

            app.draw_about();

            ImGui::End();
        };

        result.driver = app.run();
        return result;
    };

    return driver.run({ argc, argv });
}
