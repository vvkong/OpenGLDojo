
#version 330 core
in vec2 ourTexCord;
out vec4 color;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
void main()
{
    color = mix(texture(ourTexture1, ourTexCord), texture(ourTexture2, vec2(ourTexCord.x, 1-ourTexCord.y)), 0.2);
}


