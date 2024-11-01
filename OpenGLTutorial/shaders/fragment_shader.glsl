#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

// lights
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform vec3 viewPos;
uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// functions
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcAmbient(vec3 ambient);
vec3 CalcDiffuse(vec3 diffuse, vec3 lightDir, vec3 normal);
vec3 CalcSpecular(vec3 specular, vec3 lightDir, vec3 normal, vec3 viewDir);
float CalcAttenuation(float constant, float linear, float quadratic, float distance);
float CalcIntensity(SpotLight light, vec3 lightDir);

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
   /* float depth = LinearizeDepth(gl_FragCoord.z) / 10.0f;
    FragColor = vec4(vec3(depth), 1.0f);
    return;*/

    vec3 result = vec3(0.0f);
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // directional lighting
    result += CalcDirLight(dirLight, normal, viewDir);

    // point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir);
    }

    // spotLight
    result += CalcSpotLight(spotLight, normal, FragPos, viewDir);

    FragColor = vec4(result, 1.0f);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

	// ambient
    vec3 ambientComponent = CalcAmbient(light.ambient);

    //diffuse: depends on light direction and normal
    vec3 diffuseComponent = CalcDiffuse(light.diffuse, lightDir, normal);

    //specular: depends on deviation from "in=out" direction
    vec3 specularComponent = CalcSpecular(light.specular, lightDir, normal, viewDir);

    return ambientComponent + diffuseComponent + specularComponent;

}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcAmbient(vec3 ambient) {
    return ambient * vec3(texture(material.diffuse, TexCoords));
}

vec3 CalcDiffuse(vec3 diffuse, vec3 lightDir, vec3 normal) {
    float diffuseStrength = max(dot(normal, lightDir), 0.0f);
    return diffuse * diffuseStrength * vec3(texture(material.diffuse, TexCoords));
}

vec3 CalcSpecular(vec3 specular, vec3 lightDir, vec3 normal, vec3 viewDir) {
    vec3 specularReflectionDirection = reflect(lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, specularReflectionDirection), 0.0f), material.shininess);
    return specular * specularStrength * vec3(texture(material.specular, TexCoords));
}

float CalcAttenuation(float constant, float linear, float quadratic, float distance) {
    return 1.0 / (constant + linear * distance + quadratic * (distance * distance));
}

float CalcIntensity(SpotLight light, vec3 lightDir) {
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    return clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
}