#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VertexData{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} OutData;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main(){
	OutData.FragPos = vec3(model * vec4(aPos, 1.0));
	OutData.Normal = transpose(inverse(mat3(model))) * aNormal;
	OutData.TexCoords = aTexCoords;
	OutData.FragPosLightSpace = lightSpaceMatrix * vec4(OutData.FragPos, 1.0);
	gl_Position = projection * view * vec4(OutData.FragPos, 1.0);
}