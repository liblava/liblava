/**
 * @file         liblava-stage/main.cpp
 * @brief        Main stage driver
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/engine/driver.hpp>

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    return lava::driver::instance().run({ argc, argv });
}
