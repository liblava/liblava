// file      : tests/driver.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <iostream>
#include <tests/driver.hpp>

using namespace lava;

int run(int argc, char** argv) {
    auto& tests = driver::instance().get();

    argh::parser cmd_line(argc, argv);

    if (cmd_line[{ "-t", "--tests" }]) {
        for (auto& t : tests)
            std::cout << t.first << " - " << t.second->descr << std::endl;

        return to_i32(tests.size());
    }

    if (cmd_line.pos_args().size() > 1) {
        char* end_ptr = nullptr;
        auto selected = std::strtol(str(cmd_line.pos_args().at(1)), &end_ptr, 10);
        if (*end_ptr != '\0') {
            std::cerr << "wrong arguments" << std::endl;
            return test_result::wrong_arguments;
        }

        if (!tests.count(selected)) {
            std::cerr << "test " << selected << " not found" << std::endl;
            return test_result::not_found;
        }

        if (tests.count(selected))
            return tests.at(selected)->on_func(cmd_line);
    }

    for (auto& t : reverse(tests)) {
        std::cout << "test " << t.first << " - " << t.second->descr << std::endl;
        return t.second->on_func(cmd_line);
    }

    std::cerr << "no tests" << std::endl;
    return test_result::no_tests;
}

int main(int argc, char* argv[]) {
    return run(argc, argv);
}

namespace lava {

    test::test(ui32 id, name descr, func func)
    : id(id), descr(descr), on_func(func) {
        driver::instance().add_test(this);
    }

} // namespace lava
