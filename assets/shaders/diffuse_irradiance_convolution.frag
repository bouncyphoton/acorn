#version 330 core
layout (location = 0) out vec3 oFragColor;

const float PI = 3.1415926535897932384626433832795028841971693993751058;

in VertexData {
    vec3 dir;
} i;

uniform samplerCube uSky;

void main() {
    vec3 N = normalize(i.dir);
    vec3 up = vec3(0, 1, 0);
    vec3 right = cross(up, N);
    up = cross(N, right);

    vec3 irradiance = vec3(0);
    int num_samples = 0;
    for (float phi = 0; phi < PI * 2; phi += 0.025) {
        for (float theta = 0; theta < PI * 0.5; theta += 0.025) {
            vec3 temp = cos(phi) * right + sin(phi) * up;
            vec3 sampleVec = cos(theta) * N + sin(theta) * temp;
            irradiance += texture(uSky, sampleVec).rgb * cos(theta) * sin(theta);
            ++num_samples;
        }
    }

    oFragColor = irradiance * PI / num_samples;
}
