//
//  ModelA.cpp
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

#include "OpenMesh/Core/IO/MeshIO.hh"

#define SHADERS_DIR "shaders/"

Model::Model() :
_vao(0), _vbo(0)
{
}

Model::~Model()
{
	if (_vao != 0)
		glDeleteVertexArrays(1, &_vao);
	if (_vbo != 0)
		glDeleteBuffers(1, &_vbo);
}

bool Model::init(const std::string& mesh_filename)
{
	programManager::sharedInstance()
	.createProgram("default",
				   SHADERS_DIR "SimpleShader.vert",
				   SHADERS_DIR "SimpleShader.frag");

	GLuint program = programManager::sharedInstance().programWithID("default");
		
	// Obtain uniform variable handles:
	_fillColorUV  = glGetUniformLocation(program, "fillColor");

	// Load mesh
	if (!OpenMesh::IO::read_mesh(_mesh, mesh_filename.c_str()))
	{
		// if we didn't make it, exit...
		return false;
	}





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
	return true;
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


	// Draw using the state stored in the Vertex Array object:
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 0);
	glBindVertexArray(0);

	// Cleanup, not strictly necessary
	glUseProgram(0);
}

void Model::resize(int width, int height)
{
    _width	= width;
    _height = height;

    // Calculate the number of vertices to use for the new resolution
    //int num_vertices = std::max(static_cast<int>(_width * _height / VERTEX_RATIO), MIN_VERTICES);
    //this->make_vertex_array(num_vertices);
}
