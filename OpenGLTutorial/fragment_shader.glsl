#version 330 core
in vec3 outColor;
out vec4 FragColor;

// uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main()
{
    FragColor = vec4(outColor, 1.0);
}