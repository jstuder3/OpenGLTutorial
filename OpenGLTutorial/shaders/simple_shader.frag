#version 330 core
out vec4 FragColor;

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos; 
	vec3 FragPos;
} InData;

void main()
{   FragColor = vec4(vec3(0.5f), 1.0f);
}