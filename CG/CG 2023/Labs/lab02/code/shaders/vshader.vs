#version 330 core

layout (location = 0) in vec3 vVertex;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main() {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
}
