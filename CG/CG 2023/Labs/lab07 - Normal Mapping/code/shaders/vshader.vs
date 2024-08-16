#version 330 core

in vec3 vVertex;
in vec3 vTangent;
in vec3 vBitangent;
in vec3 vertex_norm;
in vec2 aTexCoord;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;
uniform vec3 lpos_world;
uniform vec3 eye_normal;

out vec3 n;
out vec3 e;
out vec3 l;
out vec2 TexCoord;
out vec3 tangent;
out vec3 bitangent;

void main() {
	gl_Position = vProjection * vView * vModel * vec4(vVertex, 1.0);
	n = normalize(vertex_norm);
    l = normalize(lpos_world - vVertex);
    e = eye_normal;
    TexCoord = vec2(aTexCoord);
    tangent = vTangent;
    bitangent = vBitangent;
}
