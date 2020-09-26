#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 inNormal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 normal;
out vec3 fragPos;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    // normal = inNormal;
    normal = mat3(transpose(inverse(model))) * inNormal;
    fragPos = vec3(model * vec4(position, 1.0f));
}