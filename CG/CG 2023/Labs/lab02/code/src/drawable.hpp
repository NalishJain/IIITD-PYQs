#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <GL/glew.h>

/*
Abstract drawable interface for all the elements drawn on the screen.
vao, vbo - respective vertex array object and vertex buffer object.
setup() - sets up the vbo for rendering
draw() - calls gl_draw for the vao.
*/

class Drawable
{
    protected:
    //VAO to store VBO
    GLuint *vao;
    //VBO to store vertices
    GLuint *vbo;
    //EBO to store indices in vertices
    GLuint *ebo;

    public:
    virtual void setup() = 0;
    virtual void draw(unsigned int& shader_program) = 0;

};
#endif