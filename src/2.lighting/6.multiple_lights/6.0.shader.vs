#version 330 core
layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 inTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec2 texCoords;
out vec3 fragPos;

void main() {
    texCoords = inTexCoords;
    fragPos = vec3(model * vec4(inPos, 1.0f));
    normal = mat3(transpose(inverse(model))) * inNormal;
    gl_Position = projection * view * vec4(fragPos, 1.0f);
}