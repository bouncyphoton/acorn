#version 330 core

out VertexData {
    vec2 uv;
} o;

vec4 vertices[4] = vec4[4](
vec4(-1.0, -1.0,  1.0, 1),
vec4(1.0, -1.0,  1.0, 1),
vec4(-1.0,  1.0,  1.0, 1),
vec4(1.0,  1.0,  1.0, 1)
);

void main() {
    gl_Position = vertices[gl_VertexID];
    o.uv = gl_Position.xy / 2.0 + 0.5;
}
