#version 450 core

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform uPushConstant
{
    vec2 uResolution;
    vec2 uTimeDepth;
    vec4 uColor;
} pc;

// Barberella ... by Weyland Yutani, dedicated to Jane Fonda and Verner Panton
// Based on Metatunnel by FRequency, really old, might not work on your gpu
// https://www.shadertoy.com/view/XdfGDr
//
// Modified by Lava Block for liblava demonstration project (lava lamp)

float h(vec3 q) // distance function
{
    float f=1.;
    // blobs
    f*=distance(q,vec3(-sin(pc.uTimeDepth.x*.181)*.5,sin(pc.uTimeDepth.x*.253),1.));
    f*=distance(q,vec3(-sin(pc.uTimeDepth.x*.252)*.5,sin(pc.uTimeDepth.x*.171),1.));
    f*=distance(q,vec3(-sin(pc.uTimeDepth.x*.133)*.5,sin(pc.uTimeDepth.x*.283),1.));
    f*=distance(q,vec3(-sin(pc.uTimeDepth.x*.264)*.5,sin(pc.uTimeDepth.x*.145),1.));
    // room
    f*=(cos(q.y))*(cos(q.z)+1.)*(cos(q.x+cos(q.z*3.))+1.)-.21+cos(q.z*6.+pc.uTimeDepth.x/20.)*cos(q.x*5.)*cos(q.y*4.5)*.3;
    return f;
}

void main()
{
    vec2 p=-1.+2.*gl_FragCoord.xy / pc.uResolution.xy;
    vec3 o=vec3(p.x,p.y*1.25-0.3,0.);
    vec3 d=vec3(p.x+cos(pc.uTimeDepth.x/20.)*0.3,p.y,1.)/64.;
    vec4 c=vec4(0.);
    float t=0.;
    for(int i=0;i<25;i++) // march
    {
        if(h(o+d*t)<.4)
        {
            t-=5.;
            for(int j=0;j<5;j++) { if(h(o+d*t)>.4) t+=1.; } // scatter
            vec3 e=vec3(.01,.0,.0);
            vec3 n=vec3(.0);
            n.x=h(o+d*t)-h(vec3(o+d*t+e.xyy));
            n.y=h(o+d*t)-h(vec3(o+d*t+e.yxy));
            n.z=h(o+d*t)-h(vec3(o+d*t+e.yyx));
            n=normalize(n);
            c+=max(dot(vec3(.0,.0,-.15),n),.0)+max(dot(vec3(.0,-.15,.15),n),.0)*.155;
            break;
        }
        t+=5.;
    }
    fragColor=c+vec4(pc.uColor.r,pc.uColor.g,pc.uColor.b,pc.uColor.a)*(t*pc.uTimeDepth.y); // fleshtones
}
