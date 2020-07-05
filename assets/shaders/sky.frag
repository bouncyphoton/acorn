#version 330 core

in VertexData {
    vec3 dir;
} i;

uniform samplerCube uEnvMap;
uniform float uExposure;

vec3 tonemap(vec3 x) {
    x *= uExposure;
    x = pow(x, vec3(1/2.2));

    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    gl_FragColor = vec4(tonemap(texture(uEnvMap, i.dir).rgb), 1);
}
