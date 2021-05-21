@ECHO on

glslangValidator -V -o fragment.spirv lamp.frag
glslangValidator -V -o vertex.spirv lamp.vert
