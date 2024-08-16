#include "mesh.hpp"

void Mesh::setup()
{
    vao = new GLuint;
    vbo = new GLuint;
    ebo = new GLuint;

    //Generating and binding vao
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    //Generate vbo for vertices
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);

    //Bind data to vbo
    glBufferData(GL_ARRAY_BUFFER, NUM_V * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    //Generate ebo for indices
    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_F * 3 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void Mesh::draw(unsigned int &shader_program)
{
    glUseProgram(shader_program);
    unsigned int vColor_uniform = getUniform(shader_program, "color");
    glUniform3f(vColor_uniform, 0.5, 0.5, 0.5);

    unsigned int vModel_uniform = getUniform(shader_program, "vModel");
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

	glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER,*vbo);

	unsigned int vVertex_attrib = getAttrib(shader_program, "vVertex");
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*ebo);
	glDrawElements(GL_TRIANGLES, NUM_F * 3, GL_UNSIGNED_INT, nullptr);

    drawEdges(shader_program);

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Mesh::drawEdges(unsigned int &shader_program)
{
    glUseProgram(shader_program);
    unsigned int vColor_uniform = getUniform(shader_program, "color");
    glUniform3f(vColor_uniform, 0.0, 0.0, 0.0);

    glBindVertexArray(*vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*ebo);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, NUM_F * 3, GL_UNSIGNED_INT, nullptr);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glUseProgram(0);


}