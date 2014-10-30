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
#include <cstdlib>

#define SHADERS_DIR "shaders/"

static const int CIRCLE_RESOLUTION = 200;
static const float BASE_RADIUS = 0.1;
static const int SQUARE_SIZE = 0;
static const float STEP_SIZE = 0.02;

Model::Model() :
_vao(0), _vbo(0), _vertex_num(CIRCLE_RESOLUTION)
{

}

Model::~Model()
{
	if (_vao != 0)
		glDeleteVertexArrays(1, &_vao);
	if (_vbo != 0)
		glDeleteBuffers(1, &_vbo);
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
	_offsetUV = glGetUniformLocation(program, "offset");

	// Initialize vertices buffer and transfer it to OpenGL
	{
		// For this example we create a single triangle:
		float* vertices = new float[this->_vertex_num * 4];
		float angle_between_vertices = (2 * M_PI) / (CIRCLE_RESOLUTION);
		for (int i = 0; i < CIRCLE_RESOLUTION; ++i)
		{
			vertices[4*i] = cosf(angle_between_vertices * i);
			vertices[4*i + 1] = sinf(angle_between_vertices * i);
			vertices[4*i + 2] = 0;
			vertices[4*i + 3] = 1;
		}
		
		// Create and bind the object's Vertex Array Object:
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		
		// Create and load vertex data into a Vertex Buffer Object:
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, this->_vertex_num * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
		
		// Tells OpenGL that there is vertex data in this buffer object and what form that vertex data takes:

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

	if (_balls.empty())
	{
		// Initialize the first ball
		this->add_ball(_width / 2, _height / 2);
	}

	for (std::vector<Ball>::iterator ball = _balls.begin(); ball != _balls.end(); ++ball)
	{
		glUniform4f(_fillColorUV, ball->_red, ball->_green, ball->_blue, 1.0);
		glUniform1i(_squareSizeUV, SQUARE_SIZE);
		glUniform1f(_radiusUV, ball->_cur_radius);
		glUniform2f(_offsetUV, ball->_x, ball->_y);

		// Draw using the state stored in the Vertex Array object:
		glBindVertexArray(_vao);

		glDrawArrays(GL_TRIANGLE_FAN, 0, this->_vertex_num);

		// Move the ball
		ball->move(STEP_SIZE);
	}
	
	// Unbind the Vertex Array object
	glBindVertexArray(0);
	
	// Cleanup, not strictly necessary
	glUseProgram(0);
}

static float get_random(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(max-min)));
}

void Model::add_ball(int x, int y)
{
	float new_x = (2*x / _width) - 1;
	float new_y = ((_height - 2*y) / _height);
	_balls.push_back(Ball(new_x,
			              new_y,
			              get_random(0, 2*M_PI),
			              BASE_RADIUS,
			              get_random(0.1,0.8),
			              get_random(0.1,0.8),
			              get_random(0.1,0.8)));
}

void Model::resize(int width, int height)
{
    _width	= width;
    _height = height;
    _offsetX = 0;
    _offsetY = 0;
}
