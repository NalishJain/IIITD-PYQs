#version 330 core

in vec3 n;
in vec3 e;
// in vec3 l;
uniform vec3 lpos_world;
out vec4 outColor;

vec3 Ls = vec3(1.0, 1.0, 1.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.6, 0.3, 0.4);

vec3 ks = vec3(1.0, 1.0, 1.0);
vec3 kd = vec3(0.5, 0.6, 0.4);
vec3 ka = vec3(1.0, 1.0, 1.0);

vec3 eye = normalize(e);
vec3 l = normalize(lpos_world - gl_FragCoord.xyz);
float spec_exp = 5;

//ambient
vec3 Ia = ka * La;

//diffuse
vec3 Id = kd * Ld * max(dot(n, l), 0.0);

//specular
vec3 Is = ks * Ls * pow(max(dot(n, normalize(eye + l)), 0.0), spec_exp);

vec3 fColor = Ia + Id + Is;
// vec3 fColor = vec3(0.0, 0.0, 0.0);

void main(void) {
        outColor = vec4(fColor, 1.0);
}
