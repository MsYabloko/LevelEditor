#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 projection;
uniform mat4 view;

out vec3 normal;

void main()
{
    normal = aNormal;
    gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}