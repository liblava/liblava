#!/bin/bash

glslangValidator -V -x -o triangle.frag.u32 triangle.frag
glslangValidator -V -x -o triangle.vert.u32 triangle.vert

glslangValidator -V -o fragment.spirv triangle.frag
glslangValidator -V -o vertex.spirv triangle.vert
