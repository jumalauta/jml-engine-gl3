#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform vec3 borderColor = vec3(0.2, 0.2, 0.2);
uniform vec3 fillColor = vec3(0.4, 0.4, 0.4);
uniform vec3 clearColor = vec3(0.0,0.0,0.0);
uniform sampler2D texture0;
uniform vec4 color = vec4(1.0,1.0,1.0,1.0);

uniform float percent = 0.0;
uniform float fadeThreshold = 0.95;

vec4 drawRectangle(vec2 position, vec2 size, vec4 color) {
    vec4 fill = vec4(clearColor,0.0);
    if (texCoord.s > position.x
            && texCoord.t > position.y
            && texCoord.s < position.x+size.x
            && texCoord.y < position.y+size.y) {
        fill = color;
    }
    return fill;
}

vec4 drawLoaderBar() {
    float fadeOut = 1.0;
    if (percent > fadeThreshold) {
        fadeOut = clamp(1.0 - ((percent - fadeThreshold) / (1.0 - fadeThreshold)), 0.0, 1.0);
    }

    float borderSizeW = 0.01;
    float aspectRatio = 16.0/9.0;
    float borderSizeH = borderSizeW * aspectRatio;

    vec4 fg = drawRectangle(vec2(0.2 + borderSizeW, 0.4 + borderSizeH), vec2((0.6 - borderSizeW*2) * percent, 0.2 - borderSizeH*2.0), vec4(fillColor, fadeOut));
    if (fg.a > 0.0) {
        return fg;
    }
    
    return drawRectangle(vec2(0.2, 0.4), vec2(0.6, 0.2), vec4(borderColor, fadeOut));
}

void main() {
    fragColor = drawLoaderBar();
}
