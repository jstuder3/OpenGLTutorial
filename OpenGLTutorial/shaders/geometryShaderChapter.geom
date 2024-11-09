#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
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

vec4 explode(vec4 position, vec3 normal){
	float magnitude = 2.0f;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}


void main(){
	vec3 normal = GetNormal();

	gl_Position = explode(gl_in[0].gl_Position, normal);
	OutData.TexCoords = InData[0].TexCoords;
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, normal);
	OutData.TexCoords = InData[1].TexCoords;
	EmitVertex();

	gl_Position = explode(gl_in[2].gl_Position, normal);
	OutData.TexCoords = InData[2].TexCoords;
	EmitVertex();

	EndPrimitive();

}