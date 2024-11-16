#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D renderedTexture;

void main(){
    FragColor = vec4(texture(renderedTexture, TexCoords).rgb, 1.0f);
}