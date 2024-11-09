#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
} InData[];

out VertexData {
	vec2 TexCoords;
	vec3 Normal;
	vec3 WorldPos;
} OutData;

const float MAGNITUDE = 0.2f;

uniform mat4 projection;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(InData[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();

    vec3 shiftDirection = normalize(vec3(- (InData[index].Normal.y / InData[index].Normal.x), 1.0, 0.0));
    gl_Position = projection * (gl_in[index].gl_Position + (vec4(InData[index].Normal, 0.0) / 6.0 * 5.0 + vec4(shiftDirection, 0.0) / 6.0) * MAGNITUDE);
    EmitVertex(); 

    gl_Position = projection * (gl_in[index].gl_Position + (vec4(InData[index].Normal, 0.0) / 6.0 * 5.0 + vec4(-shiftDirection, 0.0) / 6.0) * MAGNITUDE);
    EmitVertex();

    gl_Position = projection * (gl_in[index].gl_Position + vec4(InData[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}