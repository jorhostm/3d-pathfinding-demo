#pragma once
#include <GL/glew.h>
#include <vector>
class Cube
{
public:
	unsigned int vao;
public:
	Cube(std::vector<float>& colour);
	~Cube();
};

