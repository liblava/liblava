/**
 * @file         tests/driver.cpp
 * @brief        Test driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <iostream>
#include <tests/driver.hpp>

namespace lava {

/**
 * @brief Run the driver
 *
 * @param argc    Number of command line arguments
 * @param argv    Command line arguments
 *
 * @return int    Result code
 */
int run(int argc, char** argv) {
    auto& tests = driver::instance().get();

    argh::parser cmd_line(argc, argv);

    if (cmd_line[{ "-ts", "--tests" }]) {
        for (auto& t : tests)
            std::cout << t.first << " = " << t.second->descr << std::endl;

        return to_i32(tests.size());
    }

    if (auto test = -1; cmd_line({ "-t", "--test" }) >> test) {
        if (!tests.count(test)) {
            std::cerr << "test " << test << " not found" << std::endl;
            return test_result::not_found;
        }

        if (tests.count(test))
            return tests.at(test)->on_func(cmd_line);
    }

    for (auto& t : reverse(tests)) {
        std::cout << "test " << t.first << " - " << t.second->descr << std::endl;
        return t.second->on_func(cmd_line);
    }

    std::cerr << "no tests" << std::endl;
    return test_result::no_tests;
}

//-----------------------------------------------------------------------------
test::test(ui32 id, name descr, func func)
: id(id), descr(descr), on_func(func) {
    driver::instance().add_test(this);
}

} // namespace lava

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    return lava::run(argc, argv);
}