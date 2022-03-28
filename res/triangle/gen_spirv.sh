#!/bin/bash

glslangValidator -V -x -o fragment.u32 triangle.frag
glslangValidator -V -x -o vertex.u32 triangle.vert
