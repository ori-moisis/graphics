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

class Model {
	
	GLuint _vao, _vbo;

	// Attribute handle:
	GLint _posAttrib;
	
	// Uniform handle:
	GLint _fillColorUV;
	GLint _squareSizeUV;
	GLint _radiusUV;
	
	// View port frame:
	float _width, _height, _offsetX, _offsetY;

	// Number of vertices on the circle's edge
	int _vertex_num;

public:
	Model();

	virtual ~Model();

	// Create a circle vertex array
	void make_vertex_array(int num_vertices);

	void init();
	
	void draw();
	
	void resize(int width, int height);
	
};

#endif /* defined(__ex0__Model__) */
