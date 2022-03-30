#version 430 core

layout(location=0) out vec4 out_color;

in vec4 interColor;
in vec4 interPos;
in vec3 interNorm;
in vec2 interUV;

struct PointLight {
    vec4 pos;
    vec4 color;
};
uniform PointLight light;

uniform sampler2D diffuseTexture;

void main() {
    vec3 N = normalize(interNorm);
    //out_color = interColor; // vec4(0.0, 1.0, 1.0, 1.0);
    vec3 L = vec3(light.pos - interPos);
    float d = length(L);
    float at = 1.0 / (d*d + 1.0);
    //out_color = vec4(at, at, at, 1.0);

    L = normalize(L);

    //N += 1.0;
    //N /= 2.0;
    //out_color = vec4(N, 1.0);

    float diffuse = max(0, dot(L, N));
    vec3 diffColor = diffuse * vec3(light.color * interColor);

    vec3 texColor = vec3(texture(diffuseTexture, interUV));
    diffColor = diffColor * texColor;

    //out_color = vec4(diffuse, diffuse, diffuse, 1.0);
    //out_color = vec4(diffColor, 1.0);

    vec3 V = normalize(-vec3(interPos));
    vec3 H = normalize(L + V);
    float spec = max(0, dot(H, N));
    float s = 10.0;
    spec = pow(spec, s);
    vec3 ks = vec3(1,1,1);
    vec3 specColor = spec * ks * vec3(light.color) * diffuse;
    //out_color = vec4(specColor, 1.0);

    vec3 finalColor = diffColor + specColor;
    out_color = vec4(finalColor, 1.0);

    // Testing textures
    


}
