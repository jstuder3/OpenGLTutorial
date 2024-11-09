#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos; 
	vec3 FragPos;
} OutData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	OutData.TexCoords = aTexCoords;
    OutData.Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(position, 1.0);
	OutData.FragPos = vec3(model * vec4(aPos, 1.0));
}