#!/bin/bash

glslangValidator -V -o fragment.spirv spawn.frag
glslangValidator -V -o vertex.spirv spawn.vert
