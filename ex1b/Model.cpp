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
#include <stdio.h>
#include <set>


#define SHADERS_DIR "shaders/"

// Minimum number of vertices per circle - regardless of resolution
static const int MIN_VERTICES = 50;
// Ratio between resolution and number of vertices per circle
static const int VERTEX_RATIO = 5000;
// Radius of the circle
static const float BASE_RADIUS = 0.1;
static const float MIN_RADIUS = 0.005;
// Speed of the balls
static const float STEP_SIZE = 0.02;
// Lighting consts
static const float LIGHT_POSITION = 0.5;
static const float LIGHT_RADIUS = 300;
static const float LIGHT_FADE_FACTOR = 0.25;
static const float SHADOW_RADIUS = 550;
static const float SHADOW_FADE_FACTOR = 2;
static const float LIGHT_SPEED = 0.02;

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
	_transformMatUV = glGetUniformLocation(program, "transform");
	_lightCenterUV = glGetUniformLocation(program, "lightCenter");
	_lightRadiusUV = glGetUniformLocation(program, "lightRadius");
	_lightFadeFactorUV = glGetUniformLocation(program, "lightFadeFactor");
	_shadowRadiusUV = glGetUniformLocation(program, "shadowRadius");
	_shadowFadeFactorUV = glGetUniformLocation(program, "shadowFadeFactor");

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
	glm::vec4 lightLocation(cosf(_lightAngle) * 2, sinf(_lightAngle) * 2, 0, 1);

	// Draw the balls
	glUniform1f(_lightFadeFactorUV, LIGHT_FADE_FACTOR);
	glUniform1f(_shadowFadeFactorUV, SHADOW_FADE_FACTOR);
	for (std::vector<Ball>::iterator ball = _balls.begin(); ball != _balls.end(); ++ball)
	{
		glUniform4f(_fillColorUV, ball->_red, ball->_green, ball->_blue, 1.0f);

		glm::vec4 light_center = lightLocation - ball->_position;
		light_center /= glm::length(light_center);
		light_center *= ball->_cur_radius * LIGHT_POSITION;
		light_center += ball->_position;

		glUniform4f(_lightCenterUV, get_screen_x(light_center[0]), get_screen_y(light_center[1]), 0, 1);

		glUniform1f(_lightRadiusUV, LIGHT_RADIUS * ball->_cur_radius);
		glUniform1f(_shadowRadiusUV, SHADOW_RADIUS * ball->_cur_radius);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(ball->_position.x, ball->_position.y, 0));
		transform = glm::scale(transform,glm::vec3(ball->_cur_radius * std::min(1.0f, _height / _width),
									     	 	   ball->_cur_radius * std::min(1.0f, _width / _height),
									     	 	   1));

		glUniformMatrix4fv(_transformMatUV, 1, GL_FALSE, glm::value_ptr(transform));

		// Draw using the state stored in the Vertex Array object:
		glBindVertexArray(_vao);

		glDrawArrays(GL_TRIANGLE_FAN, 0, this->_vertex_num);
	}
	
	// Unbind the Vertex Array object
	glBindVertexArray(0);
	
	// Cleanup, not strictly necessary
	glUseProgram(0);
}

float Model::get_logical_x(float screen_x)
{
	return (2*screen_x / _width) - 1;
}

float Model::get_logical_y(float screen_y)
{
	return ((_height - 2*screen_y) / _height);
}

float Model::get_screen_x(float logical_x)
{
	return (logical_x + 1) * _width / 2;
}

float Model::get_screen_y(float logical_y)
{
	return _height - ((_height * (1-logical_y)) / 2);
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
	int strong_color = get_random(0,3);
	float red = get_random(0.0, 0.5);
	float green = get_random(0.0, 0.5);
	float blue = get_random(0.0, 0.5);

	switch(strong_color % 3)
	{
	case 0:
		red = 1;
		break;
	case 1:
		green = 1;
		break;
	case 2:
		blue = 1;
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

			if (!balls_to_skip.empty() && balls_to_skip.find(ball._id) == balls_to_skip.end())
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
	return std::max(std::min(ball._max_radius, avaliable_radius), MIN_RADIUS);
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
