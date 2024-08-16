#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include "shape.h"
class SkyBox : public Shape
{
private:
	GLuint VAO;
	GLuint vertex_VBO;
	GLint vModel_uniform, vView_uniform, vProjection_uniform;
	GLint vVertex_attrib;
	GLuint texCube;
	GLint cube_map_uniform;
	glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations

	void getShaderVariables();
	void setShaderVariables();
	void createGeometry(float size);
	void setupMVP(int screen_width, int screen_height);
public:
	SkyBox(int width, int height, float size, const char *vshaderfile, const char* fshaderfile, GLuint *texCube_ref);
	~SkyBox();
	void render();
	void setProjectionMatrix(int width, int height);
	void rotate(const float angle, const glm::vec3 &axis_in_camera_coord);
};

#endif


