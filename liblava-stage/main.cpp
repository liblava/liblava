/**
 * @file         liblava-stage/main.cpp
 * @brief        Main stage driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <iostream>
#include <liblava/lava.hpp>

using namespace lava;

/**
 * @brief Run result
 */
struct run_result {
    /// Result of driver
    i32 driver = 0;

    /// Selected stage
    i32 selected = 0;
};

/**
 * @brief Run the driver
 *
 * @param driver         Driver to run
 * @param argh           Command line arguments
 *
 * @return run_result    Run result
 */
run_result run(driver& driver, argh::parser argh) {
    run_result result;

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
                                           id, stage->descr);

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
}

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    auto& driver = driver::instance();

    driver.on_run = [&](argh::parser argh) -> i32 {
        run_result result;

        auto const& stages = driver.get_stages();

        do {
            result = run(driver, argh);
            if (result.driver < 0)
                return driver::error::undef_run;

            if (stages.count(result.selected)) {
                auto& stage = stages.at(result.selected);

                std::cout << "stage " << result.selected
                          << " - " << stage->descr << std::endl;

                auto stage_result = stage->on_func(argh);
                if (stage_result < 0)
                    return stage_result;

                if (result.selected != 0)
                    std::cout << "stage driver" << std::endl;
            }
        } while (result.selected != 0);

        return 0;
    };

    return driver.run({ argc, argv });
}
