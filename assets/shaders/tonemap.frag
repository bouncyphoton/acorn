#version 330 core
layout (location = 0) out vec4 oFragColor;

in VertexData {
    vec2 uv;
} i;

uniform sampler2D uImage;
uniform float uExposure;

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 aces(vec3 x) {
    // apply camera exposure
    x *= uExposure;

    // gamma correct
    x = pow(x, vec3(1/2.2));

    // tonemap
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 color = texture(uImage, i.uv).rgb;
    
    color = aces(color);
    
    oFragColor = vec4(color, 1);
}
