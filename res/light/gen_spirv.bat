@ECHO on

glslangValidator -V -o gbuffer.fragment.spirv gbuffer.frag
glslangValidator -V -o gbuffer.vertex.spirv gbuffer.vert

glslangValidator -V -o lighting.fragment.spirv lighting.frag
glslangValidator -V -o lighting.vertex.spirv lighting.vert
