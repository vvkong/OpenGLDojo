
#version 330 core
in vec3 ourColor;
in vec2 ourTexCord;
out vec4 color;
uniform sampler2D ourTexture1;
void main()
{
    color = texture(ourTexture1, ourTexCord) * vec4(ourColor, 1.0);
}


