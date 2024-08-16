#version 330 core

in vec3 vVertex;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 vColor;

out vec3 fColor;

void main() {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	fColor = vColor; 
	// fColor = vec3(0.2, 0.2, 0.2);
}