#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VertexData {
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
} OutData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	OutData.FragPos = vec3(model * vec4(aPos, 1.0f));
	OutData.TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	OutData.TBN = mat3(T, B, N);

}