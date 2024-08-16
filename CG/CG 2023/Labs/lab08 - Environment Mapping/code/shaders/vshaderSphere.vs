#version 330 core
in vec3 vVertex;
in vec3 vNormal;
//uniform samplerCube cube_map;
//uniform mat4 model_inv;
uniform mat4 vView;
uniform mat4 vProjection;

out vec3 fVertex;
out vec3 fNormal;

#define VERTEX_OFFSET_Z 3

void main(void) {
	fVertex = vVertex + vec3(0, 0, VERTEX_OFFSET_Z);
	fNormal = normalize(vNormal);
	gl_Position = vProjection * vView * vec4(vVertex, 1.0);
}
