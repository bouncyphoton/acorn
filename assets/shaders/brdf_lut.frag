#version 330 core
layout (location = 0) out vec2 oFragColor;

const float PI = 3.1415926535897932384626433832795028841971693993751058;

in VertexData {
    vec2 uv;
} i;

// Smith's approximation for G
float geometry_schlick_ggx(float NoV, float roughness) {
    float r = roughness;
    float k = (r * r) / 2.0;

    float denominator = (NoV) * (1 - k) + k;

    return NoV / denominator;
}

float geometry_smith(vec3 L, vec3 V, vec3 N, float roughness) {
    float ggx1 = geometry_schlick_ggx(max(0.0, dot(N, L)), roughness);
    float ggx2 = geometry_schlick_ggx(max(0.0, dot(N, V)), roughness);

    return ggx1 * ggx2;
}

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

// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
void main() {
    float NoV = i.uv.x;
    float roughness = i.uv.y;

    vec3 V = vec3(
        sqrt(1 - NoV * NoV), // sin
        0,
        NoV                  // cos
    );
    vec3 N = vec3(0, 0, 1);

    float A = 0;
    float B = 0;

    const uint num_samples = 1024u;
    for (uint i = 0u; i < num_samples; ++i) {
        vec2 Xi = hammersley_2d(i, num_samples);
        vec3 H  = importance_sample_ggx(Xi, roughness, N);
        vec3 L  = 2 * dot(V, H) * H - V;

        float NoL = max(0.0, L.z);
        float NoH = max(0.0, H.z);
        float VoH = max(0.0, dot(V, H));

        if (NoL > 0) {
            float G = geometry_smith(L, V, N, roughness);

            float GVis = (G * VoH) / (NoH * NoV);
            float Fc = pow(1 - VoH, 5.0);
            A += (1 - Fc) * GVis;
            B += Fc * GVis;
        }
    }

    // A is a scale value
    // B is a bias value
    oFragColor = vec2(A, B) / num_samples;
}
