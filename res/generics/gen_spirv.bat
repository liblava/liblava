@ECHO on

glslangValidator -V -o triangle_frag.spirv triangle.frag
glslangValidator -V -o lava_triangle.spirv lava_triangle.vert
glslangValidator -V -o int_triangle.spirv int_triangle.vert
glslangValidator -V -o double_triangle.spirv double_triangle.vert
