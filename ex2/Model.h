//
//  ModelA.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//

#ifndef __ex2__Model__
#define __ex2__Model__

#include <iostream>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif

#include <string>

#include <glm/glm.hpp>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> Mesh;

class Model {
	
	GLuint _vao, _vbo;

	// Attribute handle:
	GLint _posAttrib;
	
	// Uniform handle:
	GLint _modelUV;
	GLint _viewUV;
	GLint _projectionUV;
	
	// View port frame:
	float _width, _height;

	Mesh _mesh;

	glm::mat4 _model;
	glm::mat4 _view;
	glm::mat4 _projection;

public:
	Model();

	virtual ~Model();

	bool init(const std::string& mesh_filename);
	
	void draw();
	
	void resize(int width, int height);
	
};

#endif /* defined(__ex2__Model__) */
