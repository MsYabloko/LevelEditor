#version 330 core
out vec4 FragColor;

in vec2 uv;
uniform mat4 projection;
uniform mat4 view;
uniform vec2 screensize;

float grid(vec2 st, float res)
{
    vec2 grid = fract(st*res);
    return (step(res,grid.x) * step(res,grid.y));
}

void main()
{
    vec2 uvv = uv + vec2(-0.5, 1.5);
    mat4 invView = inverse(view);
    mat4 invProj = inverse(projection);
    vec4 clipspace = vec4(uvv.xy - 1.0, -1.0, 1.0);
    vec4 viewSpace = invProj * clipspace;
    viewSpace /= viewSpace.w;
    vec4 world = invView * viewSpace;

    FragColor = vec4(0, 0, 0.f, 1.0f);
    if(mod(world.x, 5) < 0.25f)
    {
        FragColor = vec4(0.5, 0.5, 0.5, 1);
    }
    if(mod(world.y, 5) < 0.25f)
    {
        FragColor = vec4(0.5, 0.5, 0.5, 1);
    }
    if(abs(world.x) < 2 || abs(world.y) < 2)
    {
        FragColor = vec4(0, 1, 0, 1);
    }
/*
    vec2 grid_uv = ((uv + vec2(0.5, 0.5)) * screensize) / length(screensize) * 150;
    float x = grid(grid_uv, 0.1);
    FragColor = vec4(vec3(0.5) * (1 - x), 1);*/

}