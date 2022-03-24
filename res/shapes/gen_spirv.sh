#!/bin/bash

glslangValidator -V -o fragment.spirv shapes.frag
glslangValidator -V -o vertex.spirv shapes.vert
