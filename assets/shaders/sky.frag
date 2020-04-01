#version 330 core

in VertexData {
    vec3 dir;
} i;

uniform samplerCube uSkybox;

void main() {
    gl_FragColor = texture(uSkybox, i.dir);
}
