#version 330 core

in vec3 fTexCoord;

uniform samplerCube cube_map;

out vec4 outColor;

void main(void) {
	outColor = texture(cube_map, fTexCoord);
}
