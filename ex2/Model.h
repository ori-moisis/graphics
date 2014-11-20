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
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
typedef OpenMesh::PolyMesh_ArrayKernelT<> Mesh;

class Model {
	
	GLuint _vao, _vbo;

	// Attribute handle:
	GLint _posAttrib;
	
	// Uniform handle:
	GLint _fillColorUV;
	
	// View port frame:
	float _width, _height;

	Mesh _mesh;

public:
	Model();

	virtual ~Model();

	bool init(const std::string& mesh_filename);
	
	void draw();
	
	void resize(int width, int height);
	
};

#endif /* defined(__ex2__Model__) */
