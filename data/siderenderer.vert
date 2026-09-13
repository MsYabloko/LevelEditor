#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aUv;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;

void main()
{
    uv = aUv;
    //mat4 newmat = (projection * view);
    //gl_Position = newmat * vec4(aPos.x, aPos.y, aPos.z, 1.0) + vec4(1,-1,0,0);
    //gl_Position = (projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0)) + vec4(1,-1,0,0);
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
