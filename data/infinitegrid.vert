#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraWorldPos;

const float gridSize = 100;

out vec3 WorldPos;

void main()
{
   WorldPos = aPos * gridSize + vec3(cameraWorldPos.x, 0, cameraWorldPos.z);
   gl_Position = projection * view * vec4(aPos * gridSize + vec3(cameraWorldPos.x, 0, cameraWorldPos.z), 1.0);
}