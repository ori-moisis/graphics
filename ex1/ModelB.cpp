//
//  ModelB.cpp
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//

#include "ShaderIO.h"
#include "ModelB.h"

#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif // WIN32

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
#include <stdio.h>
#include <set>
#include <cmath>
#include <algorithm>


#define SHADERS_DIR "shadersB/"

// Minimum number of vertices per circle - regardless of resolution
static const int MIN_VERTICES = 50;
// Ratio between resolution and number of vertices per circle
static const int VERTEX_RATIO = 5000;
// Radius of the circle
static const float BASE_RADIUS = 0.1f;
static const float MIN_RADIUS = 0.0f;
// Speed of the balls
static const float STEP_SIZE = 0.02f;
// Rotation speed of the light source
static const float LIGHT_SPEED = 0.02f;
// Number of balls to draw in each call to glDrawArraysInstanced
static const int M = 32;

Model::Model() :
_vao(0), _vbo(0), _vertex_num(0), _lightAngle(0)
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
	float angle_between_vertices = (2.0f * M_PI) / (num_vertices);
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
	_transformMatUV = glGetUniformLocation(program, "transform");
	_lightLocationUV = glGetUniformLocation(program, "lightLocation");
	_resolutionUV = glGetUniformLocation(program, "resolution");
	_perspectiveUV = glGetUniformLocation(program, "perspective");

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

	if (_balls.empty())
	{
		// Initialize the first ball
		this->add_ball(_width / 2, _height / 2);
	}

	// Move all balls
	for (std::vector<Ball>::iterator ball = _balls.begin(); ball != _balls.end(); ++ball)
	{
		ball->move(STEP_SIZE, _width / _height);
	}

	// Check collisions and fix radius if needed
	std::set<size_t> balls_to_skip;
	for (std::vector<Ball>::iterator ball = _balls.begin(); ball != _balls.end(); ++ball)
	{
		ball->_cur_radius = get_new_radius(*ball, balls_to_skip);
	}

	// Calculate light source location
	_lightAngle += LIGHT_SPEED;
	glm::vec4 lightLocation(cosf(_lightAngle) * 0.5, sinf(_lightAngle) * 0.5, 0, 1);

	// Set current resolution
	glUniform2f(_resolutionUV, _width, _height);
	// Set current light source location
	glUniform4fv(_lightLocationUV, 1, glm::value_ptr(lightLocation));

	float aspect = _height / _width;
	glm::mat4 pers = glm::scale(glm::mat4(1.0f), glm::vec3(sqrt(aspect), sqrt(1 / aspect), 1));
	glUniformMatrix4fv(_perspectiveUV, 1, GL_FALSE, glm::value_ptr(pers));

	// Draw using the state stored in the Vertex Array object:
	glBindVertexArray(_vao);

	// Draw the balls
	glm::mat4 transform[M];
	glm::vec4 color[M];
	int i = 0;
	for (std::vector<Ball>::iterator ball = _balls.begin(); ball != _balls.end(); ++ball)
	{
		transform[i] = glm::translate(glm::mat4(1.0f), glm::vec3(ball->_position.x, ball->_position.y, 0));
		transform[i] = glm::scale(transform[i],glm::vec3(ball->_cur_radius, ball->_cur_radius, 1));
		color[i] = glm::vec4(ball->_red, ball->_green, ball->_blue, 1.0f);

		if (++i == M)
		{
			// Flush balls
			glUniformMatrix4fv(_transformMatUV, M, GL_FALSE, glm::value_ptr(transform[0]));
			glUniform4fv(_fillColorUV, M, glm::value_ptr(color[0]));
			glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, this->_vertex_num, M);
			i = 0;
		}
	}
	
	if (i > 0)
	{
		// Some unflushed balls remain
		glUniformMatrix4fv(_transformMatUV, i, GL_FALSE, glm::value_ptr(transform[0]));
		glUniform4fv(_fillColorUV, i, glm::value_ptr(color[0]));
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, this->_vertex_num, i);
	}

	// Unbind the Vertex Array object
	glBindVertexArray(0);
	
	// Cleanup, not strictly necessary
	glUseProgram(0);
}

float Model::get_logical_x(float screen_x)
{
	return ((2*screen_x / _width) - 1) * sqrt(_width / _height);
}

float Model::get_logical_y(float screen_y)
{
	return ((_height - 2*screen_y) / _height) * sqrt(_height / _width);
}

static float get_random(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(max-min)));
}

void Model::add_ball(float x, float y)
{
	// Get logical coordinates
	float logical_x = get_logical_x(x);
	float logical_y = get_logical_y(y);

	// set radius w.r.t nearby walls
	float max_radius = std::min(BASE_RADIUS,
								std::min(std::min(std::abs(1 - logical_x), std::abs(1 + logical_x)),
										 std::min(std::abs(1 - logical_y), std::abs(1 + logical_y))));

	// Randomize dominant color and color mix
	int strong_color = rand() % 3;
	float red = get_random(0.0, 0.3);
	float green = get_random(0.0, 0.3);
	float blue = get_random(0.0, 0.3);

	switch(strong_color % 3)
	{
	case 0:
		red = 0.8;
		break;
	case 1:
		green = 0.8;
		break;
	case 2:
		blue = 0.8;
		break;
	}

	// Prevent new balls inside existing ones
	for (std::vector<Ball>::iterator ball = _balls.begin(); ball != _balls.end(); ++ball)
	{
		glm::vec4 new_center(logical_x, logical_y, 0, 1);
		if (glm::distance(new_center, ball->_position) < ball->_cur_radius)
		{
			return;
		}
	}

	// Create new ball object
	Ball ball(_balls.size(),
			  logical_x,
		      logical_y,
		      get_random(0, 2*M_PI),
		      max_radius,
		      red,
		      green,
		      blue);

	// Set size according to surrounding balls. No need to skip existing balls
	std::set<size_t> skip;
	ball._max_radius = get_new_radius(ball, skip);

	_balls.push_back(ball);
}

float Model::get_new_radius(const Ball& ball, std::set<size_t>& balls_to_skip)
{
	float avaliable_radius = ball._max_radius;
	for (std::vector<Ball>::iterator ball2 = _balls.begin(); ball2 != _balls.end(); ++ball2)
	{
		if (ball._id != ball2->_id)
		{
			// Get the distance from my center to the other ball's edge
			float dist_to_edge = glm::distance(ball._position, ball2->_position) - ball2->_cur_radius;
			float radius_decrease = ball._cur_radius - dist_to_edge;

			if (balls_to_skip.find(ball._id) == balls_to_skip.end())
			{
				// This is the first time we see these two balls collide
				// split the radius decrease evenly between the two balls
				radius_decrease /= 2;
			}

			if (ball._cur_radius - radius_decrease < avaliable_radius)
			{
				// This is an actual collision, mark the other ball so it will
				// fully decrease its radius
				balls_to_skip.insert(ball2->_id);
				avaliable_radius = ball._cur_radius - radius_decrease;
			}
		}
	}
	return std::max(avaliable_radius, MIN_RADIUS);
}

void Model::resize(int width, int height)
{
    _width	= width;
    _height = height;
    _offsetX = 0;
    _offsetY = 0;

    int num_vertices = std::max(static_cast<int>(_width * _height / VERTEX_RATIO), MIN_VERTICES);
    this->make_vertex_array(num_vertices);
}
