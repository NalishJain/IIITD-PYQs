#version 330 core

in vec3 n; // Normal Vector
in vec3 e; // Eye vector
in vec3 l; // Light vector
in vec2 TexCoord; // Texture coordinates
in vec3 tangent;
in vec3 bitangent;
out vec4 outColor;

uniform sampler2D texture1; // color texture
uniform sampler2D normalMap; // normal map

vec3 Ls = vec3(1.0, 1.0, 1.0); // Specular light color
vec3 Ld = vec3(0.5, 0.6, 0.4); // Diffuse light color
vec3 La = vec3(0.6, 0.3, 0.4); // Ambient light color

vec3 ks = vec3(1.0, 1.0, 1.0); // Specular reflection constant
vec3 kd = vec3(0.5, 0.6, 0.4); // Diffuse reflection constant
vec3 ka = vec3(1.0, 1.0, 1.0); // Ambient reflection constant

float spec_exp = 3.5;

void main(void) {
    // To-Do: Construct the TBN matrix
    mat3 TBN;
    TBN[0] = normalize(tangent);
    TBN[1] = normalize(bitangent);
    TBN[2] = normalize(n);
    TBN = transpose(TBN);

    // To-Do: Load and transform the normal from the normal map into tangent space
    // vec3 normal = normalize(n); // Comment to remove normal mapping
    vec3 normal = normalize(texture(normalMap, TexCoord).rgb * 2.0 - 1.0);

    // To-Do: Transform the light vector and eye vector into tangent space
    // vec3 tLight = normalize(l);
    // vec3 tView = normalize(e);
    vec3 tLight = normalize(TBN * l);
    vec3 tView = normalize(TBN * e);
    

    // Ambient component
    vec3 Ia = ka * La;

    // Diffuse component
    vec3 Id = kd * max(dot(normal, tLight), 0.0) * Ld;

    // Specular component
    vec3 r = normalize(reflect(-tLight, normal));
    vec3 h = normalize(tLight + tView);
    vec3 Is = ks * Ls * pow(max(dot(h, normal), 0.0), spec_exp);

    // Combine the components
    vec3 fColor = Ia + Id + Is;

    // Final color
    outColor = 0.3 * vec4(fColor, 1.0) + 0.7 * texture(texture1, TexCoord);
}
