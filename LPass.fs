#version 430 core

layout(location=0) out vec4 out_color;

in vec2 interUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct PointLight {
    vec4 pos;
    vec4 color;
};
const int LIGHT_CNT = 10;
uniform PointLight lights[LIGHT_CNT];

void main() {
    vec3 interPos = vec3(texture(gPosition, interUV));
    vec3 N = vec3(texture(gNormal, interUV));
    vec4 albedoSpec = texture(gAlbedoSpec, interUV);
    vec3 albedo = albedoSpec.rgb;
    float shininess = albedoSpec.a * 400.0;

    vec3 V = normalize(-interPos);
    vec3 finalColor = vec3(0,0,0);

    for(int i = 0; i < LIGHT_CNT; i++) {
        vec3 L = normalize(vec3(lights[i].pos) - interPos);
        float diffCoef = max(0, dot(L, N));
        vec3 diffColor = diffCoef * vec3(lights[i].color) * albedo;
        vec3 H = normalize(L + V);
        float specCoef = pow(max(0, dot(H, N)), shininess);
        vec3 specColor = specCoef*vec3(lights[i].color)*diffCoef;
        finalColor += diffColor + specColor;
    }

    finalColor = finalColor / (finalColor + vec3(1.0));
    out_color = vec4(finalColor, 1.0);
}
