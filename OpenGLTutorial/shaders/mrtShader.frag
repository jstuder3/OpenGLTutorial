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
uniform bool invertedNormals;

void main() {
	gPosition = InData.FragPos;
	gNormal = (invertedNormals ? -1.0f : 1.0f) * normalize(InData.Normal);
	gAlbedoSpec.rgb = invertedNormals ? vec3(1.0f) : texture(material.diffuse, InData.TexCoords).rgb;
	gAlbedoSpec.a = invertedNormals ? 0.0f : texture(material.specular, InData.TexCoords).r;
}