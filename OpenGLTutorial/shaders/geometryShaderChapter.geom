#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
} InData[];
out VertexData {
	vec2 TexCoords;
	vec3 Normal;
} OutData;

uniform float time;

out vec3 fColor;

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

vec4 explode(vec4 position, vec3 worldPos, vec3 normal){
	float magnitude = 0.5f;
	vec3 direction = normal * (clamp(sin(time*4 + worldPos.x), 0, 1)) * magnitude;
	return position + vec4(direction, 0.0);
}


void main(){
	vec3 normal = GetNormal();

	gl_Position = explode(gl_in[0].gl_Position, InData[0].WorldPos, normal);
	OutData.TexCoords = InData[0].TexCoords;
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, InData[0].WorldPos, normal);
	OutData.TexCoords = InData[1].TexCoords;
	EmitVertex();

	gl_Position = explode(gl_in[2].gl_Position, InData[0].WorldPos, normal);
	OutData.TexCoords = InData[2].TexCoords;
	EmitVertex();

	EndPrimitive();

}