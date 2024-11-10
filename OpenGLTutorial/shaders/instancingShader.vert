#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
	vec3 FragPos;
} OutData;

uniform mat4 projection;
uniform mat4 view;

uniform float time;
uniform mat4 additionalRotate;
uniform mat4 additionalRotate2;

void main(){
	mat4 additionalRotate2Adjusted = additionalRotate2;
    if(gl_InstanceID%2 == 0){
		additionalRotate2Adjusted = transpose(additionalRotate2Adjusted);
	}
	if(gl_InstanceID%5 == 0){
		additionalRotate2Adjusted = additionalRotate2Adjusted * additionalRotate2Adjusted;
	}
	gl_Position = projection * view * additionalRotate * instanceMatrix  * additionalRotate2Adjusted * vec4(aPos+vec3(cos(time + gl_InstanceID) * 2, sin(time + gl_InstanceID) * 2, 0.0f), 1.0f);
	OutData.Normal = normalize(mat3(transpose(inverse(additionalRotate * instanceMatrix  * additionalRotate2))) * aNormal);
	OutData.TexCoords = aTexCoords;
	OutData.FragPos = vec3(instanceMatrix * vec4(aPos, 1.0f));
}