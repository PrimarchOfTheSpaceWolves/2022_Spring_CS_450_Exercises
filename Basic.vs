#version 430 core

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 texcoords;
layout(location=4) in vec3 tangent;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat3 normMat;

out vec4 interColor;
out vec4 interPos;
out vec3 interNorm;
out vec2 interUV;
out vec3 interTangent;

void main() {
    vec4 pos = vec4(position, 1.0);
    vec4 vpos = viewMat * modelMat * pos;
    interPos = vpos;
    gl_Position = projMat * vpos;
    interColor = color;
    interNorm = normMat*normal;
    interUV = texcoords;

    interTangent = vec3(viewMat * modelMat * vec4(tangent, 0));
}
