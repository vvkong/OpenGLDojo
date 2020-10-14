#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

in vec3 fragPosition;
in vec2 texCoords;
in vec3 normal;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 result;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 norm = normalize(normal);

    for( int i=0; i<NR_POINT_LIGHTS; i++ ) {
        result += calcPointLight(pointLights[i], material, norm, fragPosition, viewDir);
    }

    color = vec4(result, 1.0f);
}

vec3 calcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat.shininess);

    float distance = length(light.position-fragPos);
    float attention = 1.0f/(light.constant + light.linear * distance + light.quadratic * distance*distance);

    vec3 ambient = light.ambient * vec3(texture(mat.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(mat.specular, texCoords));

    return (ambient + diffuse + specular) * attention;
}