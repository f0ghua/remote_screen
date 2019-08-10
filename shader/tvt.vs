#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 coor;

out vec2 Coor;

void main()
{
    vec3 p = aPos;
    p.z -= 1;
    gl_Position =  vec4(p, 1.0);
    Coor = coor;
}



