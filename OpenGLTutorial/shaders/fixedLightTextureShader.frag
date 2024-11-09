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

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
    vec3 FragPos;
} InData;

out vec4 FragColor;

void main(){

    vec3 viewDir = normalize(viewPos - InData.FragPos);
    vec3 lightDir = normalize(-dirLight.direction);

    vec3 outColor = vec3(0.0f); 
    //ambient component
    outColor += dirLight.ambient * vec3(texture(material.diffuse, InData.TexCoords));

    // diffuse component
    float diffuseStrength = max(dot(InData.Normal, lightDir), 0.0f);
    outColor += diffuseStrength * dirLight.diffuse * vec3(texture(material.diffuse, InData.TexCoords));

    //specular component
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specStrength = pow(max(dot(InData.Normal, halfwayDir), 0.0), 8.0);
    outColor += specStrength * dirLight.specular * vec3(texture(material.specular, InData.TexCoords));

    FragColor = vec4(outColor, 1.0);
	
}

