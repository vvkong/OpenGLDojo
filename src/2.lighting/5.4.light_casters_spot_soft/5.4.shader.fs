#version 330 core
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material material;
uniform Light light;
uniform vec3 viewPos;
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;
out vec4 color;
void main()
{
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta-light.outerCutOff)/epsilon, 0.0f, 1.0f);

    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));   

    vec3 viewDir = normalize(viewPos-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    // 镜面贴图，本身让周边反光，中间木头不反光
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    
    // 翻转镜面贴图颜色值，木头也反光
    // vec4 tmpSpecTexture = texture(material.specular, texCoords);
    // vec3 specular = light.specular * spec * vec3(1.0f-tmpSpecTexture.r, 1.0f-tmpSpecTexture.g, 1.0f-tmpSpecTexture.b);

    // 放射光
    vec3 emission = texture(material.emission, texCoords).rgb;
    
    vec3 result = ambient + (diffuse + specular + emission)*intensity;
    color = vec4(result, 1.0f);
}