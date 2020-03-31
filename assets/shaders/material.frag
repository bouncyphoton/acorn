#version 330 core
layout (location = 0) out vec4 oFragColor;

const float PI = 3.1415926535897932384626433832795028841971693993751058;

in VertexData {
    vec3 position;
    vec3 normal;
    vec2 uv;
    mat3 tbn;
} i;

uniform struct {
    sampler2D albedo;
    sampler2D normal;
    sampler2D metallic;
    sampler2D roughness;
} uMaterial;

uniform vec3 uSunDirection;
uniform vec3 uCameraPosition;

float ggx_distribution(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float den = (NdotH2 * (a2 - 1.0) + 1.0);
    den = PI * den * den;

    return nom / den;
}

vec3 fresnel_schlick(float cos_theta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}

vec3 fresnel_schlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float geometry_schlick_ggx(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_ggx(NdotV, roughness);
    float ggx1 = geometry_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}

//---------------
// calculate brdf with cook-torrance for specular and lambert for diffuse
//---------------
vec3 calculate_brdf(vec3 albedo, vec3 N, vec3 V, float metallic, float roughness) {
    vec3 Wo = V; // outgoing light direction
    vec3 Wi = uSunDirection; // incoming light direction
    vec3 H = normalize(V + Wi); // halfway vector
    vec3 F0 = mix(vec3(0.04), albedo, metallic); // material response at normal incidence

    float D = ggx_distribution(N, H, roughness); // distribution
    vec3  F = fresnel_schlick(max(dot(H, Wo), 0), F0); // fresnel
    float G = geometry_smith(N, Wo, Wi, roughness); // geometry

    vec3 specular = (D * F * G) / (4 * max(0, dot(Wo, N)) * max(0, dot(Wi, N)) + 0.001);

    // TODO: proper Ks
    vec3 Ks = F; // amount of specularly reflected light

    vec3 Kd = (1 - Ks) * (1 - metallic); // amount of diffused incoming radiance

    // TODO: proper Li
    vec3 Li = vec3(1);

    return (Kd * albedo / PI + /*Ks **/ specular) * Li * max(0, dot(N, Wi));
}

void main() {
    if (texture(uMaterial.albedo, i.uv).a == 0) discard;

    vec3 albedo = pow(texture(uMaterial.albedo, i.uv).rgb, vec3(2.2));
    vec3 normal = normalize(i.tbn * (texture(uMaterial.normal, i.uv).rgb * 2 - 1));
    vec3 view_dir = normalize(uCameraPosition - i.position);
    float metallic = texture(uMaterial.metallic, i.uv).r;
    float roughness = texture(uMaterial.roughness, i.uv).r;

    vec3 color = calculate_brdf(albedo, normal, view_dir, metallic, roughness);

    oFragColor = vec4(pow(color, vec3(1/2.2)), texture(uMaterial.albedo, i.uv).a);
}
