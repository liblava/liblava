#!/bin/bash

glslangValidator -V -x -o imgui.frag.u32 imgui.frag
glslangValidator -V -x -o imgui.vert.u32 imgui.vert

glslangValidator -V -o fragment.spirv imgui.frag
glslangValidator -V -o vertex.spirv imgui.vert
