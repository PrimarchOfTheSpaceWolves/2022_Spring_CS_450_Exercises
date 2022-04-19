#version 430 core

layout(location=0) out vec3 gPosition;
layout(location=1) out vec3 gNormal;
layout(location=2) out vec4 gAlbedoSpec;

in vec4 interColor;
in vec4 interPos;
in vec3 interNorm;
in vec2 interUV;
in vec3 interTangent;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform float shininess;

void main() {
    vec3 N = normalize(interNorm);
        
    vec3 T = normalize(interTangent);
    T = normalize(T - dot(T,N)*N);
    vec3 B = normalize(cross(N,T));
    vec3 texN = vec3(texture(normalTexture, interUV));
    texN.x = texN.x*2.0f - 1.0f;
    texN.y = texN.y*2.0f - 1.0f;
    texN = normalize(texN);
    mat3 toView = mat3(T,B,N);
    //N = normalize(toView*texN);
    
    vec3 texColor = vec3(texture(diffuseTexture, interUV));
    vec3 diffColor = vec3(interColor) * texColor;

    gPosition = vec3(interPos);
    gNormal = N;
    gAlbedoSpec.rgb = diffColor;
    gAlbedoSpec.a = shininess/400.0;    
}
