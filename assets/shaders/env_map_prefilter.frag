#version 330 core
layout (location = 0) out vec3 oFragColor;

const float PI = 3.1415926535897932384626433832795028841971693993751058;

in VertexData {
    vec3 dir;
} i;

uniform samplerCube uEnvMap;
uniform float uRoughness;

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radical_inverse_van_der_corput(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley_2d(uint i, uint N) {
    return vec2(float(i)/float(N), radical_inverse_van_der_corput(i));
}

vec3 importance_sample_ggx(vec2 Xi, float roughness, vec3 N) {
    float a = roughness * roughness;

    float phi = 2 * PI * Xi.x;
    float cos_theta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
    float sin_theta = sqrt(1 - cos_theta * cos_theta);

    vec3 H = vec3(
        sin_theta * cos(phi),
        sin_theta * sin(phi),
        cos_theta
    );

    vec3 up = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
    vec3 tan_x = normalize(cross(up, N));
    vec3 tan_y = cross(N, tan_x);
    // tangent to world sapce
    return tan_x * H.x + tan_y * H.y + N * H.z;
}

void main() {
    vec3 N = normalize(i.dir);

    vec3 color = vec3(0);
    float total_weight = 0;

    const uint num_samples = 1024u;
    for (uint i = 0u; i < num_samples; ++i) {
        vec2 Xi = hammersley_2d(i, num_samples);
        vec3 H = importance_sample_ggx(Xi, uRoughness, N);
        vec3 L = 2 * dot(N, H) * H - N;

        float NdotL = max(0, dot(N, L));

        if (NdotL > 0) {
            color += texture(uEnvMap, L).rgb * NdotL;
            total_weight += NdotL;
        }
    }

    oFragColor = color / total_weight;
}
