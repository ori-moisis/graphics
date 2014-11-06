//
//  Model.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//

#ifndef __ex0__Model__
#define __ex0__Model__

#include <iostream>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

#include <set>
#include <vector>
#include "Ball.h"

class Model {
	
	GLuint _vao, _vbo;


	// Attribute handle:
	GLint _posAttrib;
	
	// Uniform handle:
	GLint _fillColorUV;
	GLint _transformMatUV;
	GLint _lightCenterUV;
	GLint _lightRadiusUV;
	GLint _lightFadeFactorUV;
	GLint _shadowRadiusUV;
	GLint _shadowFadeFactorUV;

	
	// View port frame:
	float _width, _height, _offsetX, _offsetY;

	// Number of vertices on the circle's edge
	int _vertex_num;

	// All balls
	std::vector<Ball> _balls;

	// Current light source location
	float _lightAngle;

public:
	Model();
	virtual ~Model();

	// Create a circle vertex array
	void make_vertex_array(int num_vertices);

	void init();
	
	void draw();

	// Calculate logical coordinates from screen position
	float get_logical_x(float screen_x);
	float get_logical_y(float screen_y);
	// Calculate screen position from logical coordinates
	float get_screen_x(float logical_x);
	float get_screen_y(float logical_y);
	
	// Add a ball to the screen
	void add_ball(float x, float y);

	// Get a new radius for the ball according to the balls around it
	float get_new_radius(const Ball& ball, std::set<size_t>& balls_to_skip);

	// Resize window callback
	void resize(int width, int height);
	
};

#endif /* defined(__ex0__Model__) */
