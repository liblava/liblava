/**
 * @file         liblava/engine/main.cpp
 * @brief        Engine main
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/engine/driver.hpp>

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    return lava::run({ argc, argv });
}
