#version 430 core

layout(location=0) out vec4 out_color;

uniform vec4 lightColor;

void main() {
    out_color = lightColor;
}
