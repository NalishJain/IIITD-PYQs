#version 330 core
in vec3 vVertex;
uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

out vec3 fTexCoord;
void main(void) {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	fTexCoord = vVertex;
}
