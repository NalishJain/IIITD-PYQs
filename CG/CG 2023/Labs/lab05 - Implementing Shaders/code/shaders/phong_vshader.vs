#version 330 core

in vec3 vVertex;
in vec3 vNormal;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 lpos_world;
uniform vec3 eye_normal;

out vec3 n;
out vec3 e;
// out vec3 l;

void main() {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	n = normalize(vNormal);
    // l = normalize(lpos_world - vVertex);
    e = eye_normal;


}