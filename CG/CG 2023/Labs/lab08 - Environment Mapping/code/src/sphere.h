#ifndef _SPHERE_H_
#define _SPHERE_H_
#include "shape.h"

class Sphere : public Shape
{
private:
	GLuint VAO;
	GLuint vertex_VBO, normal_VBO, indices_IBO;
	GLint vModelInv_uniform, vView_uniform, vProjection_uniform;
	GLint vVertex_attrib, vNormal_attrib;
	GLuint texCube;
	GLint cube_map_uniform;
	glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations

	void getShaderVariables();
	void setShaderVariables();
	void createGeometry(float size);
	void setupMVP(int screen_width, int screen_height);

public:
	Sphere(int width, int height, float size, const char *vshaderfile, const char* fshaderfile, GLuint *texCube_ref);
	~Sphere();
	void render();
	void setProjectionMatrix(int width, int height);
	void rotate(const float angle, const glm::vec3 &axis_in_camera_coord);
};
#endif
