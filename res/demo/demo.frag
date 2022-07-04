#version 450 core

layout(binding = 0) uniform sampler2D screenshot;

layout(location = 0) out vec4 outFragColor;

layout(push_constant) uniform uPushConstant {
    vec2 resolution;
    float time;
    float alpha;
}
pc;

// Mationi - Colored Border by juanpetrik
// https://www.shadertoy.com/view/fscyDH
//
// Modified by Lava Block for liblava demonstration project (lava demo)

void main() {
    vec2 uv = gl_FragCoord.xy / pc.resolution;
    vec4 base = texture(screenshot, uv);

    vec2 p1 = vec2(0.01, 0.01);
    vec2 p2 = vec2(0.99, 0.99);
    vec2 p3 = vec2(0.01, 0.99);
    vec2 p4 = vec2(0.99, 0.01);

    vec4 c2 = texture(screenshot, uv + pc.time/10.0);
    
    float d1 = step(p1.x,uv.x)*step(uv.x,p4.x)*abs(uv.y-p1.y)+
        step(uv.x,p1.x)*distance(uv,p1)+step(p4.x,uv.x)*distance(uv,p4);
    d1 = min(step(p3.x,uv.x)*step(uv.x,p2.x)*abs(uv.y-p2.y)+
        step(uv.x,p3.x)*distance(uv,p3)+step(p2.x,uv.x)*distance(uv,p2),d1);
    d1 = min(step(p1.y,uv.y)*step(uv.y,p3.y)*abs(uv.x-p1.x)+
        step(uv.y,p1.y)*distance(uv,p1)+step(p3.y,uv.y)*distance(uv,p3),d1);
    d1 = min(step(p4.y,uv.y)*step(uv.y,p2.y)*abs(uv.x-p2.x)+
        step(uv.y,p4.y)*distance(uv,p4)+step(p2.y,uv.y)*distance(uv,p2),d1);
        
    float f1 = 0.01 / abs(d1 + c2.r/100.0);
    
    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(pc.time+uv.xyx+vec3(0,2,4));
    
	outFragColor = mix(vec4(f1 * col, 1.0), base, 0.9);
    outFragColor.a = pc.alpha;
}
