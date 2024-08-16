#include "utils.h"
#include "sphere.h"
#include <stdlib.h>

Sphere::Sphere(int width, int height, float size, const char *vshaderfile, const char* fshaderfile, GLuint *texCube_ref)
{
	program = createProgram(vshaderfile, fshaderfile);
	getShaderVariables();
	texCube = *texCube_ref;

	//Create geometry
	createGeometry(size);

	//Setup transformations - MVP matrices
	setupMVP(width, height);

	//Set values to the shader values: attributes, and uniform
	setShaderVariables();
}

Sphere::~Sphere()
{
	glDeleteProgram(program);
}

void Sphere::getShaderVariables()
{
	//TODO: DONE Get location of normal attribute variable, inverse of modeling transform, and the cube map texture
	
	glUseProgram(program);
	

	//Attribute variables
	vVertex_attrib = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}
	
	//Import normals
	
	vNormal_attrib = glGetAttribLocation(program, "vNormal");
	if(vNormal_attrib == -1) {
		fprintf(stderr, "Could not bind location: vNormal\n");
		exit(0);
	}

	//Uniform variables
	//Import cubemap
	cube_map_uniform = glGetUniformLocation(program, "cube_map");
	if(cube_map_uniform == -1){
		fprintf(stderr, "Could not bind location: cube_map\n");
		exit(0);
	}
	
	//Import inverse modelling transform
	vModelInv_uniform = glGetUniformLocation(program, "model_inv");
	if(vModelInv_uniform == -1){
		fprintf(stderr, "Could not bind location: model_inv\n");
		exit(0);
	}
	
	vView_uniform = glGetUniformLocation(program, "vView");
	if(vView_uniform == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}

	vProjection_uniform = glGetUniformLocation(program, "vProjection");
	if(vProjection_uniform == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}
}

void Sphere::setShaderVariables()
{
	glUseProgram(program);
	
	//TODO: Set value of the inverse of modelling transformation and cube map sampler
	glUniformMatrix4fv(vModelInv_uniform, 1, GL_FALSE, glm::value_ptr(glm::inverse(modelT))); 

	//vVertex, vNormal are set during VBO creation

	//Projection matrix
	glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT)); 
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT)); 
}

void Sphere::createGeometry(float size)
{
	glUseProgram(program);

	//Sphere data: use spherical parameterization to produce points
	//(x,y,z) = (r sin th cos ph, t sin th sin ph, r cos th), th \in [0, pi], ph \in [0, 2 pi]
	//Generate vertices and normals.
#define DELTA_ANGLE 3
#define VERTEX_OFFSET_Z 3
	int nTheta = 180/DELTA_ANGLE + 1;
	int nPhi = 360/DELTA_ANGLE + 1;
	float *vertices = new float[nTheta*nPhi*3];
	float *normals = new float[nTheta*nPhi*3];
	float theta, phi, x, y, z;
	float radius = 1.0f;
	for (int j = 0; j<nTheta; j++)
		for(int i=0; i<nPhi; i++)
		{
			theta = float(j*DELTA_ANGLE)*M_PI/180.0;
			phi = float(i*DELTA_ANGLE)*M_PI/180.0;
			x = sinf(theta)*cosf(phi);
			y = sinf(theta)*sinf(phi);
			z = cos(theta);
			normals[(i + j*nPhi)*3 + 0] = x; normals[(i + j*nPhi)*3 + 1] = y; normals[(i + j*nPhi)*3 + 2] = z; 
			vertices[(i + j*nPhi)*3 + 0] = radius*x; vertices[(i + j*nPhi)*3 + 1] = radius*y; vertices[(i + j*nPhi)*3 + 2] = radius*z - VERTEX_OFFSET_Z; 
		}
		
	//Generate index array
	GLushort *indices = new GLushort[2*(nTheta-1)*(nPhi-1)*3];
	for(int j=0; j<(nTheta-1); j++)
		for(int i=0; i<(nPhi-1); i++)
		{
			//Upper triangle
			indices[(i + j*(nPhi-1))*6 + 0] = i + j*nPhi;
			indices[(i + j*(nPhi-1))*6 + 1] = i + (j+1)*nPhi;
			indices[(i + j*(nPhi-1))*6 + 2] = i + 1 + j*nPhi;

			//Lower triangle
			indices[(i + j*(nPhi-1))*6 + 3] = i + 1 + j*nPhi;
			indices[(i + j*(nPhi-1))*6 + 4] = i + (j+1)*nPhi;
			indices[(i + j*(nPhi-1))*6 + 5] = i + 1 + (j+1)*nPhi;
		}

	//Generate vertex array object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertex_VBO);
	glGenBuffers(1, &normal_VBO);
	glGenBuffers(1, &indices_IBO);

	glBindVertexArray(VAO);

	//Generate vertex buffer and index buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, nTheta*nPhi*3*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	delete []vertices;

	glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
	glBufferData(GL_ARRAY_BUFFER, nTheta*nPhi*3*sizeof(GLfloat), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vNormal_attrib);
	glVertexAttribPointer(vNormal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	//TODO: DONE Enable normal attrib and describe layout with glVertexAttribPointer
	delete []normals;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (nTheta-1)*(nPhi-1)*6*sizeof(GLushort), indices, GL_STATIC_DRAW);
	delete []indices;

	glBindVertexArray(0);
}


void Sphere::setupMVP(int screen_width, int screen_height)
{
	glUseProgram(program);

	//Modelling transformations (Model -> World coordinates)
	modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

	//Viewing transformations (World -> Camera coordinates
	viewT = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));
	//Camera at (0, 0, 0) looking down the negative Z-axis in a right handed coordinate system

	//Set projection matrix
	setProjectionMatrix(screen_width, screen_height);
}

void Sphere::setProjectionMatrix(int width, int height)
{
	glUseProgram(program);

	//Projection transformation (Orthographici/Perspective projection)
	float aspect = (float)width/(float)height;
	float view_height = 100.0f;
	//glm::mat4 projection = glm::ortho(-view_height*aspect/2.0f, view_height*aspect/2.0f, -view_height/2.0f, view_height/2.0f, 0.1f, 1000.0f);
	projectionT = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT)); 
}
void Sphere::render()
{
	//TODO: Attach normal VBO and Pass on normal attribute to the shader
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);

	glBindVertexArray(VAO);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

//Roate the skybox to see around the scene on mouse drag
void Sphere::rotate(const float angle, const glm::vec3 &axis_in_camera_coord)
{
	//TODO: DONE Update inverse modelling transformation to the shader
	glUseProgram(program);
	glm::mat3 camera2object = glm::inverse(glm::mat3(viewT*modelT));
	glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;

	modelT = glm::rotate(modelT, angle, axis_in_object_coord);
	setShaderVariables();
}
