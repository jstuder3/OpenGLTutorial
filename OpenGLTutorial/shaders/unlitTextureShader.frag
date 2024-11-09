#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
} InData;

out vec4 FragColor;

void main(){
	FragColor = texture(material.diffuse, InData.TexCoords);
}

