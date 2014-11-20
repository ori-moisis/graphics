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

#define SHADERS_DIR "shaders/"

static const float OBJECT_DEPTH = 10;
static const float OBJECT_B_RAD = 2;

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

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthRange(-OBJECT_B_RAD, OBJECT_B_RAD);
		
	// Obtain uniform variable handles:
	_modelUV = glGetUniformLocation(program, "model");
	_viewUV = glGetUniformLocation(program, "view");
	_projectionUV = glGetUniformLocation(program, "projection");

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

	// Load mesh
	if (!OpenMesh::IO::read_mesh(_mesh, mesh_filename.c_str()))
	{
		// if we didn't make it, exit...
		return false;
	}

	const float fmax = std::numeric_limits<float>::max();
	const float fmin = std::numeric_limits<float>::min();
	Mesh::Point lowerLeft(fmax, fmax, fmax);
	Mesh::Point upperRight(fmin, fmin, fmin);
	Mesh::Point center(0,0,0);

	float* vertices = new float[_mesh.n_vertices() * 4];
	int i = 0;
	for (Mesh::VertexIter iter = _mesh.vertices_begin();
		 iter != _mesh.vertices_end();
		 ++iter, ++i)
	{
		Mesh::Point p = _mesh.point(iter);
		center += p;
		for (int j = 0; j < 3; ++j)
		{
			vertices[4*i + j] = p[j];
			lowerLeft[j] = std::min(lowerLeft[j], p[j]);
			upperRight[j] = std::max(upperRight[j], p[j]);
		}
		vertices[4*i + 3] = 1;
	}
	center /= (double)_mesh.n_vertices();


	_model = glm::translate(glm::mat4(1.0f), glm::vec3(-center[0], -center[1], -center[2]));
	_model = glm::scale(glm::mat4(1.0f), glm::vec3(2/(upperRight[0] - lowerLeft[0]),
										           2/(upperRight[1] - lowerLeft[1]),
										           2/(upperRight[2] - lowerLeft[2]))) * _model;
	//_model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,OBJECT_DEPTH)) * _model;


	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _mesh.n_vertices() * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

    delete [] vertices;

	return true;
}

void Model::draw()
{
	// Set the program to be used in subsequent lines:
	GLuint program = programManager::sharedInstance().programWithID("default");
	glUseProgram(program);

	GLenum polygonMode = GL_FILL;   // Also try using GL_FILL and GL_POINT
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	glUniformMatrix4fv(_modelUV, 1, GL_FALSE, glm::value_ptr(_model));

	static float angle = 0.0f;
	angle += 0.6f;
	_view = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.5,1,0));
	_view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, OBJECT_DEPTH)) * _view;

	glUniformMatrix4fv(_viewUV, 1, GL_FALSE, glm::value_ptr(_view));

	// Perspective
	_projection = glm::perspective(30.0f, _width / _height, OBJECT_DEPTH - OBJECT_B_RAD, OBJECT_DEPTH + OBJECT_B_RAD) *
			      glm::lookAt(glm::vec3(0,0,0) /* eye */,
            				  glm::vec3(0,0,OBJECT_DEPTH) /* center */,
            				  glm::vec3(0,1,0) /* up */);

	glUniformMatrix4fv(_projectionUV, 1, GL_FALSE, glm::value_ptr(_projection));

	// Draw using the state stored in the Vertex Array object:
	glBindVertexArray(_vao);

	std::vector<unsigned int> indices;
	for (Mesh::FaceIter iter = _mesh.faces_begin();
		 iter != _mesh.faces_end();
		 ++iter)
	{
		for (Mesh::FaceVertexIter v_iter = _mesh.fv_begin(iter); v_iter != _mesh.fv_end(iter); ++v_iter)
		{
			indices.push_back(v_iter.handle().idx());
		}
	}

	//std::reverse(indices.begin(), indices.end());


	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]);
	glBindVertexArray(0);

	// Cleanup, not strictly necessary
	glUseProgram(0);
}

void Model::resize(int width, int height)
{
    _width	= width;
    _height = height;
}
