//
//  Model.cpp
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//

#include "ShaderIO.h"
#include "Model.h"

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"

#define SHADERS_DIR "shaders/"

// Minimum number of vertices per circle - regardless of resolution
static const int MIN_VERTICES = 50;
// Ratio between resolution and number of vertices per circle
static const int VERTEX_RATIO = 5000;
// Radius of the circle
static const float BASE_RADIUS = 0.8;
// Size (pixels) of each square
static const int SQUARE_SIZE = 7;

Model::Model() :
_vao(0), _vbo(0), _vertex_num(0)
{
}

Model::~Model()
{
	if (_vao != 0)
		glDeleteVertexArrays(1, &_vao);
	if (_vbo != 0)
		glDeleteBuffers(1, &_vbo);
}

void Model::make_vertex_array(int num_vertices)
{
	this->_vertex_num = num_vertices;
	// Create vertices on the circle edge
	float* vertices = new float[this->_vertex_num * 4];
	float angle_between_vertices = (2 * M_PI) / (num_vertices);
	for (int i = 0; i < num_vertices; ++i)
	{
		vertices[4*i] = cosf(angle_between_vertices * i);
		vertices[4*i + 1] = sinf(angle_between_vertices * i);
		vertices[4*i + 2] = 0;
		vertices[4*i + 3] = 1;
	}
	// Bind to vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, this->_vertex_num * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

	delete [] vertices;
}

void Model::init()
{
	programManager::sharedInstance()
	.createProgram("default",
				   SHADERS_DIR "SimpleShader.vert",
				   SHADERS_DIR "SimpleShader.frag");

	GLuint program = programManager::sharedInstance().programWithID("default");
		
	// Obtain uniform variable handles:
	_fillColorUV  = glGetUniformLocation(program, "fillColor");
	_squareSizeUV = glGetUniformLocation(program, "squareSize");
	_radiusUV = glGetUniformLocation(program, "radius");

	// Initialize vertices buffer and transfer it to OpenGL
	{
		// Create and bind the object's Vertex Array Object:
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		
		// Create and load vertex data into a Vertex Buffer Object:
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		// Obtain attribute handles:
		_posAttrib = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(_posAttrib);
		glVertexAttribPointer(_posAttrib, // attribute handle
							  4,          // number of scalars per vertex
							  GL_FLOAT,   // scalar type
							  GL_FALSE,
							  0,
							  0);
		
		// Unbind vertex array:
		glBindVertexArray(0);
	}
}

void Model::draw()
{
	// Set the program to be used in subsequent lines:
	GLuint program = programManager::sharedInstance().programWithID("default");
	glUseProgram(program);

	GLenum polygonMode = GL_FILL;   // Also try using GL_FILL and GL_POINT
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// Set uniform variable with RGB values:
	float red = 0.3f; float green = 0.5f; float blue = 0.7f;
	glUniform4f(_fillColorUV, red, green, blue, 1.0);
	// Set square size and circle radius
	glUniform1i(_squareSizeUV, SQUARE_SIZE);
	glUniform1f(_radiusUV, BASE_RADIUS);


	// Draw using the state stored in the Vertex Array object:
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, _vertex_num);
	glBindVertexArray(0);

	// Cleanup, not strictly necessary
	glUseProgram(0);
}

void Model::resize(int width, int height)
{
    _width	= width;
    _height = height;
    _offsetX = 0;
    _offsetY = 0;

    // Calculate the number of vertices to use for the new resolution
    int num_vertices = std::max(static_cast<int>(_width * _height / VERTEX_RATIO), MIN_VERTICES);
    this->make_vertex_array(num_vertices);
}
