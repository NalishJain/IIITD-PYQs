#version 330 core
uniform vec3 color;
out vec4 fColor;

void main()

{	
	fColor = vec4(color, 1.0f);
}

