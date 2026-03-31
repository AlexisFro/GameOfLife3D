#version 430 core

layout (location = 0) in vec3 vPos;
layout (location = 2) in vec3 iOffset; // Position de l'instance
out vec3 WorldPos;
out vec3 InstancePos;
uniform mat4 VP;

void main()
{
    vec3 pos = vPos + iOffset;
    WorldPos = pos;
    InstancePos = iOffset; // Position centrale du cube pour le dégradé
    gl_Position = VP * vec4(pos, 1.0);
};