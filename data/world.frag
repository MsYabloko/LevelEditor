#version 330 core

out vec4 FragColor;
in vec3 normal;

void main()
{
    FragColor = vec4(abs(normal), 1);
}