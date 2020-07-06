#version 330 core
layout (location = 0) out vec4 oFragColor;

in VertexData {
    vec3 dir;
} i;

uniform samplerCube uEnvMap;

void main() {
    oFragColor = vec4(texture(uEnvMap, i.dir).rgb, 1);
}
