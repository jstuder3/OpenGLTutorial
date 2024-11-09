#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
} OutData;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = view * model * vec4(aPos, 1.0); 
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	OutData.Normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
	OutData.TexCoords = aTexCoords;
}