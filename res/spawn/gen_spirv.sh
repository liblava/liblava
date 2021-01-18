#!/bin/bash

glslangValidator -V -x -o spawn.frag.u32 spawn.frag
glslangValidator -V -x -o spawn.vert.u32 spawn.vert

glslangValidator -V -o fragment.spirv spawn.frag
glslangValidator -V -o vertex.spirv spawn.vert
