#version 450 core

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform uPushConstant {
    vec2 uResolution;
    vec2 uTimeDepth;
    vec4 uColor;
}
pc;

// Barberella ... by Weyland Yutani, dedicated to Jane Fonda and Verner Panton
// Based on Metatunnel by FRequency, really old, might not work on your gpu
// https://www.shadertoy.com/view/XdfGDr
//
// Modified by Lava Block for liblava demonstration project (lava lamp)

float h(vec3 q) // distance function
{
    float f = 1.;
    // blobs
    f *= distance(q, vec3(-sin(pc.uTimeDepth.x * 0.181) * 0.5, sin(pc.uTimeDepth.x * 0.253), 1.0));
    f *= distance(q, vec3(-sin(pc.uTimeDepth.x * 0.252) * 0.5, sin(pc.uTimeDepth.x * 0.171), 1.0));
    f *= distance(q, vec3(-sin(pc.uTimeDepth.x * 0.133) * 0.5, sin(pc.uTimeDepth.x * 0.283), 1.0));
    f *= distance(q, vec3(-sin(pc.uTimeDepth.x * 0.264) * 0.5, sin(pc.uTimeDepth.x * 0.145), 1.0));
    // room
    f *= (cos(q.y)) * (cos(q.z) + 1.0) * (cos(q.x + cos(q.z * 3.0)) + 1.0) - 0.21 + cos(q.z * 6.0 + pc.uTimeDepth.x / 20.0) * cos(q.x * 5.0) * cos(q.y * 4.5) * 0.3;
    return f;
}

void main() {
    vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / pc.uResolution.xy;
    vec3 o = vec3(p.x, p.y * 1.25 - 0.3, 0.0);
    vec3 d = vec3(p.x + cos(pc.uTimeDepth.x / 20.0) * 0.3, p.y, 1.0) / 64.0;
    vec4 c = vec4(0.0);
    float t = 0.0;
    for (int i = 0; i < 25; ++i) // march
    {
        if (h(o + d * t) < 0.4) {
            t -= 5.0;
            for (int j = 0; j < 5; ++j) {
                if (h(o + d * t) > 0.4)
                    t += 1.;
            } // scatter
            vec3 e = vec3(0.01, 0.0, 0.0);
            vec3 n = vec3(0.0);
            n.x = h(o + d * t) - h(vec3(o + d * t + e.xyy));
            n.y = h(o + d * t) - h(vec3(o + d * t + e.yxy));
            n.z = h(o + d * t) - h(vec3(o + d * t + e.yyx));
            n = normalize(n);
            c += max(dot(vec3(0.0, 0.0, -0.15), n), 0.0) + max(dot(vec3(0.0, -0.15, 0.15), n), 0.0) * 0.155;
            break;
        }
        t += 5.0;
    }
    fragColor = c + vec4(pc.uColor.r, pc.uColor.g, pc.uColor.b, pc.uColor.a) * (t * pc.uTimeDepth.y); // fleshtones
}
