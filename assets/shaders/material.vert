#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

out VertexData {
    vec3 position;
    vec3 normal;
    vec2 uv;
    mat3 tbn;
} o;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

void main() {
    o.position = vec3(uModelMatrix * vec4(aPosition, 1));
    o.normal = vec3(uNormalMatrix * vec4(aNormal, 1));
    o.uv = aUv;

    vec3 t = normalize(vec3(uModelMatrix * vec4(aTangent, 0)));
    vec3 b = normalize(vec3(uModelMatrix * vec4(aBiTangent, 0)));
    vec3 n = normalize(vec3(uModelMatrix * vec4(aNormal, 0)));
    o.tbn = mat3(t, b, n);

    gl_Position = uViewProjectionMatrix * vec4(o.position, 1);
}
