#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertex_colour;

uniform mat4 m_matrix;

out vec3 colour;

void main()
{
  
  gl_Position = m_matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  colour = vertex_colour;

 
}