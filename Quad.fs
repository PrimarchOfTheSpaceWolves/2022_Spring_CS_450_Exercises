#version 330 core
layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform sampler2D screenTexture;

void main() {
    vec2 UV = vec2(interUV.x, interUV.y + 0.01*sin(interUV.x*100.0));
    vec3 color = vec3(texture(screenTexture, UV));    
    out_color = vec4(color, 1.0);
}