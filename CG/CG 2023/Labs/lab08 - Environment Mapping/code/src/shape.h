#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <stdio.h>

class Shape
{
protected:
	GLuint program;

	virtual void getShaderVariables() = 0;
	virtual void setShaderVariables() = 0;
	virtual void createGeometry(float size) = 0;
public:
	virtual void render() = 0;
};
#endif
