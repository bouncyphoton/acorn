#version 330 core

out VertexData {
    vec3 dir;
} o;

uniform mat4 uViewProjectionMatrix;

vec4 vertices[8] = vec4[8](
vec4(-1.0, -1.0,  1.0, 1),
vec4(1.0, -1.0,  1.0, 1),
vec4(-1.0,  1.0,  1.0, 1),
vec4(1.0,  1.0,  1.0, 1),
vec4(-1.0, -1.0, -1.0, 1),
vec4(1.0, -1.0, -1.0, 1),
vec4(-1.0,  1.0, -1.0, 1),
vec4(1.0,  1.0, -1.0, 1)
);

int indices[14] = int[14](
    0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
);

void main() {
    vec4 clip_pos = uViewProjectionMatrix * vertices[indices[gl_VertexID]];
    gl_Position = clip_pos.xyww;
    o.dir = vertices[indices[gl_VertexID]].xyz;
}
