#!/bin/bash

glslangValidator -V -x -o lamp.frag.u32 lamp.frag
glslangValidator -V -x -o lamp.vert.u32 lamp.vert

glslangValidator -V -o fragment.spirv lamp.frag
glslangValidator -V -o vertex.spirv lamp.vert
