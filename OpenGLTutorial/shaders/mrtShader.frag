#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D height;
};

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
} InData;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;

void main() {
	gPosition = InData.FragPos;
	gNormal = normalize(InData.Normal);
	gAlbedoSpec.rgb = texture(material.diffuse, InData.TexCoords).rgb;
	gAlbedoSpec.a = texture(material.specular, InData.TexCoords).r;
}