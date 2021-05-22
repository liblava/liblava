struct UboData {
    vec3 camPos;
    uint lightCount;
    mat4 view;
    mat4 projection;
    mat4 invProjection;
    uvec2 resolution;
};

struct PushConstantData {
    mat4 model;
    vec3 color;
    float metallic;
    uint enableNormalMapping;
};

struct LightData {
    vec3 position;
    float radius;
    vec3 intensity;
    float _padding;
};
