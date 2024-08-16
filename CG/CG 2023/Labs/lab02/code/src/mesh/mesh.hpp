#ifndef MESH_H
#define MESH_H

#include "drawable.hpp"
#include <glm/glm.hpp>
#include <utils/utils.hpp>

/*
    Mesh class that takes a set of vertices and indices and renders it.
*/

class Mesh : Drawable 
{
    
private: 
    int NUM_V, NUM_F;
    //Vertices position of shape Vx3
    GLfloat *vertices;
    //Indices into vertices for triangles of shape Fx3
    GLuint *indices;
	glm::mat4 modelT;

    void drawEdges(unsigned int &shader_program);
    
public:
	//Drawable functions
	void setup();
	void draw(unsigned int &shader_program);

    Mesh(GLfloat *vertices, GLuint *indices, int nV, int nF)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->NUM_V = nV;
        this->NUM_F = nF;

        this->modelT = glm::identity<glm::mat4>();

        //Call drawable's setup
        this->setup();
    }

};

#endif