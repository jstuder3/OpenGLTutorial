#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VertexData {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} OutData;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	OutData.FragPos = vec3(mat3(model) * aPos);
	OutData.TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	vec3 T = normalize(mat3(model) * aTangent);
	vec3 N = normalize(mat3(model) * aNormal);
	// vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));

	// re-orthogonalize using Gram-Schmidt
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 reverseTBN = transpose(mat3(T, B, N));

	OutData.TangentLightPos = reverseTBN * lightPos;
	OutData.TangentViewPos = reverseTBN * viewPos;
	OutData.TangentFragPos = reverseTBN * OutData.FragPos;

}