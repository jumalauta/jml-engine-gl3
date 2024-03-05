#version 330 core

in vec2 texCoord;
in vec4 vertexFragColor;

out vec4 fragColor;
uniform sampler2D texture0; // diffuse
uniform sampler2D texture1; // ambient / depth
uniform sampler2D texture2; // specular
uniform sampler2D texture3; // normal
uniform vec4 color = vec4(1.0,1.0,1.0,1.0);
uniform bool enableVertexColor = false;

void main(void)
{
    fragColor = color;

    if (enableVertexColor) {
        fragColor *= vertexFragColor;
    }

    fragColor *= texture(texture0, texCoord);

    fragColor = clamp(fragColor, vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));
} 
