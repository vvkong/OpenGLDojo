#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCord;

out vec3 ourColor;
out vec2 ourTexCord;
void main()
{
    gl_Position = vec4(position, 1.0);
    ourColor = color;
    ourTexCord = vec2(texCord.x, texCord.y);
}

