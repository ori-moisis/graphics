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
	
	GLuint _vao[2];
	GLuint _vbo[2];

	// Attribute handle:
	GLint _posAttrib;
	
	// Uniform handle:
	GLint _modelUV;
	GLint _viewUV;
	GLint _projectionUV;
	GLint _isArcballUV;
	
	// View port frame:
	float _width, _height;

	// Current polygon mode
	GLenum _polygonMode;

	Mesh _mesh;

	// Indices for glDrawElements
	std::vector<unsigned int> _elementIndices;

	glm::mat4 _model;
	glm::mat4 _projection;

	glm::mat4 _scale;
	glm::mat4 _translate;
	glm::mat4 _rotate;

	struct MouseClickState
	{
		bool _isActive;
		glm::mat4 _transform;
		glm::vec3 _initialMouseLocation;
	};

	std::vector<MouseClickState> _mouseStates;

	enum ProjectionMode
	{
		PERSPECTIVE = 0,
		ORTHO = 1
	};

	ProjectionMode _projectionMode;


public:
	Model();

	virtual ~Model();

	bool init(const std::string& mesh_filename);
	
	void draw();
	
	void resize(int width, int height);
	
	glm::vec3 get_normalized_location(int x, int y);

	void mouse_click(int button, bool isBegin, int x, int y);

	void mouse_move(int x, int y);

	void reset();

	void switchPolygonMode();

	void switchPerspective();

};

#endif /* defined(__ex2__Model__) */
