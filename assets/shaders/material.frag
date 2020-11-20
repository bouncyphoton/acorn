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
    vec3 albedo_scale;
    sampler2D normal;
    sampler2D metallic;
    float metallic_scale;
    sampler2D roughness;
    float roughness_scale;
} uMaterial;

uniform samplerCube uDiffuseIrradianceMap;
uniform samplerCube uPrefilteredEnvironmentMap;
uniform sampler2D uBrdfLut;
uniform int uNumPrefilteredEnvMipmapLevels;

uniform vec3 uSunDirection;
uniform vec3 uCameraPosition;

const vec2 inv_atan = vec2(1.0 / (2 * PI), 1.0 / PI);
vec2 sample_equirectangular_map(vec3 v) {
    // convert from cartesian to polar to uv
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return vec2(uv.x, 1.0 - uv.y);
}

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

vec3 fresnel_schlick_roughness(float cos_theta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cos_theta, 5.0);
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

vec3 calculate_ibl(vec3 albedo, vec3 N, vec3 V, float metallic, float roughness) {
    // environment
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnel_schlick_roughness(max(0, dot(N, V)), F0, roughness);

    // diffuse
    vec3 Kd = (1 - F) * (1 - metallic);
    vec3 Li = texture(uDiffuseIrradianceMap, N).rgb;
    vec3 diffuse = Li * albedo * Kd;

    // specular, split-sum
    float NdotV = max(0, dot(N, V));
    vec3 R = reflect(-V, N);
    vec3 prefiltered_color = textureLod(uPrefilteredEnvironmentMap, R, roughness * uNumPrefilteredEnvMipmapLevels).rgb;
    vec2 env_brdf = texture(uBrdfLut, vec2(NdotV, roughness)).rg;
    vec3 specular = prefiltered_color * (F * env_brdf.x + env_brdf.y);

    return diffuse + specular;
}

//---------------
// calculate brdf for light
//---------------
vec3 calculate_brdf(vec3 albedo, vec3 N, vec3 V, float metallic, float roughness, vec3 Wi, vec3 Li) {
    vec3 Wo = V;// outgoing light direction
    vec3 H = normalize(V + Wi);// halfway vector
    vec3 F0 = mix(vec3(0.04), albedo, metallic);// material response at normal incidence

    float D = ggx_distribution(N, H, roughness);// distribution
    vec3  F = fresnel_schlick(max(dot(H, Wo), 0), F0);// fresnel
    float G = geometry_smith(N, Wo, Wi, roughness);// geometry

    vec3 specular = (D * F * G) / (4 * max(0, dot(Wo, N)) * max(0, dot(Wi, N)) + 0.001);

    vec3 Ks = F;// amount of specularly reflected light

    vec3 Kd = (1 - Ks) * (1 - metallic);// amount of diffused incoming radiance

    return (Kd * albedo / PI + specular) * Li * max(0, dot(N, Wi));
}

void main() {
    // TODO: transparency
    if (texture(uMaterial.albedo, i.uv).a <= 0.1) discard;

    vec3 albedo = pow(texture(uMaterial.albedo, i.uv).rgb * uMaterial.albedo_scale, vec3(2.2));
    vec3 normal = normalize(i.tbn * (texture(uMaterial.normal, i.uv).rgb * 2 - 1));
    vec3 view_dir = normalize(uCameraPosition - i.position);
    float metallic = texture(uMaterial.metallic, i.uv).r * uMaterial.metallic_scale;
    float roughness = texture(uMaterial.roughness, i.uv).r * uMaterial.roughness_scale;

    vec3 color = vec3(0);

    color += calculate_ibl(albedo, normal, view_dir, metallic, roughness);

    // sun light
    color += calculate_brdf(albedo, normal, view_dir, metallic, roughness, uSunDirection, vec3(1));

    oFragColor = vec4(color, texture(uMaterial.albedo, i.uv).a);
}
