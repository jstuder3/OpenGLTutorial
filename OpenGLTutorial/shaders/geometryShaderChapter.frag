#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

out vec4 FragColor;

in vec2 TexCoords;

uniform Material material;

void main()
{
    FragColor = texture(material.diffuse, TexCoords);
}
