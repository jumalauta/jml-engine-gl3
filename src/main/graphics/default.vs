#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec4 vertexColor;

out vec2 texCoord;
out vec4 vertexFragColor;
uniform mat4 mvp;

void main(void)
{
    vec4 position = vec4(vertexPosition, 1.0);
    gl_Position = mvp * position;
    texCoord = vertexTexCoord;
    vertexFragColor = vertexColor;
} 
