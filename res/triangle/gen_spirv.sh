#!/bin/bash

glslangValidator -V -o fragment.spirv triangle.frag
glslangValidator -V -o vertex.spirv triangle.vert
