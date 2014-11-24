//
//  ModelA.cpp
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2013.
//

#include "ShaderIO.h"
#include "Model.h"

#define GLM_FORCE_RADIANS

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

// Number of vertices in the arcball
static const int ARCBALL_VERTICES = 100;
static const float ARCBALL_RAD = 0.9f;


static const float OBJECT_DEPTH = 6.0f;
static const float OBJECT_B_RAD = 2.0f;


Model::Model() :
_polygonMode(GL_FILL),
_scale(1.0f),
_translate(1.0f),
_rotate(1.0f),
_mouseStates(3, MouseClickState()),
_projectionMode (PERSPECTIVE)
{
	memset(_vao, 0, sizeof(_vao));
	memset(_vbo, 0, sizeof(_vbo));
}

Model::~Model()
{
	if (_vao[0] != 0)
		glDeleteVertexArrays(2, _vao);
	if (_vbo[0] != 0)
		glDeleteBuffers(2, _vbo);
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

	// Obtain uniform variable handles:
	_modelUV = glGetUniformLocation(program, "model");
	_viewUV = glGetUniformLocation(program, "view");
	_projectionUV = glGetUniformLocation(program, "projection");
	_isArcballUV = glGetUniformLocation(program, "isArcball");

	// Obtain attribute handles:
	_posAttrib = glGetAttribLocation(program, "position");


	// Load mesh
	if (!OpenMesh::IO::read_mesh(_mesh, mesh_filename.c_str()))
	{
		// if we didn't make it, exit...
		return false;
	}

	// Create mesh vertices
	const float max_float = std::numeric_limits<float>::max();
	Mesh::Point lowerLeft(max_float, max_float, max_float);
	Mesh::Point upperRight(-max_float, -max_float, -max_float);
	Mesh::Point center(0,0,0);

	float* mesh_vertices = new float[_mesh.n_vertices() * 4];
	int i = 0;
	for (Mesh::VertexIter iter = _mesh.vertices_begin();
		 iter != _mesh.vertices_end();
		 ++iter, ++i)
	{
		Mesh::Point p = _mesh.point(*iter);
		center += p;
		for (int j = 0; j < 3; ++j)
		{
			mesh_vertices[4*i + j] = p[j];
			lowerLeft[j] = fmin(lowerLeft[j], p[j]);
			upperRight[j] = fmax(upperRight[j], p[j]);
		}
		mesh_vertices[4*i + 3] = 1;
	}
	center /= (double)_mesh.n_vertices();

	for (Mesh::FaceIter iter = _mesh.faces_begin();
		 iter != _mesh.faces_end();
		 ++iter)
	{
		Mesh::FaceVertexIter end_iter = _mesh.fv_end(*iter);
		for (Mesh::FaceVertexIter v_iter = _mesh.fv_begin(*iter); v_iter != end_iter; ++v_iter)
		{
			_elementIndices.push_back(v_iter.handle().idx());
		}
	}

	// Scale the largest dimension to -1:1
	float scale_factor = 0;
	for (int i = 0; i < 3; ++i)
	{
		scale_factor = std::max(scale_factor, upperRight[i] - lowerLeft[i]);
	}

	_model = glm::translate(glm::mat4(1.0f), glm::vec3(-center[0], -center[1], -center[2]));
	_model = glm::scale(glm::mat4(1.0f), glm::vec3(2/scale_factor)) * _model;

	// Create arcball vertices
	float* arc_vertices = new float[ARCBALL_VERTICES * 4];
	float angle_between_vertices = (2 * M_PI) / (ARCBALL_VERTICES);
	for (int i = 0; i < ARCBALL_VERTICES; ++i)
	{
		arc_vertices[4*i] = cosf(angle_between_vertices * i) * ARCBALL_RAD;
		arc_vertices[4*i + 1] = sinf(angle_between_vertices * i) * ARCBALL_RAD;
		arc_vertices[4*i + 2] = -1;
		arc_vertices[4*i + 3] = 1;
	}


	// Create the objects' Vertex Array Object:
	glGenVertexArrays(2, _vao);
	glGenBuffers(2, _vbo);

	// Create and bind the mesh VAO
	glBindVertexArray(_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);


	glEnableVertexAttribArray(_posAttrib);
	glVertexAttribPointer(_posAttrib, // attribute handle
						  4,          // number of scalars per vertex
						  GL_FLOAT,   // scalar type
						  GL_FALSE,
						  0,
						  0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, _mesh.n_vertices() * 4 * sizeof(float), mesh_vertices, GL_STATIC_DRAW);

    // Unbind vertex array:
   	//glBindVertexArray(0);

   	// Bind arcball vertex array
   	glBindVertexArray(_vao[1]);
   	glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);

	glEnableVertexAttribArray(_posAttrib);
	glVertexAttribPointer(_posAttrib, // attribute handle
						  4,          // number of scalars per vertex
						  GL_FLOAT,   // scalar type
						  GL_FALSE,
						  0,
						  0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, ARCBALL_VERTICES * 4 * sizeof(float), arc_vertices, GL_STATIC_DRAW);

	// Unbind vertex array:
	glBindVertexArray(0);

    delete [] mesh_vertices;
    delete [] arc_vertices;

	return true;
}

void Model::draw()
{
	if (_width <= 0 || _height <= 0)
	{
		// Cannot draw before we know the screen size
		return;
	}
	// Set the program to be used in subsequent lines:
	GLuint program = programManager::sharedInstance().programWithID("default");
	glUseProgram(program);

	glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

	glUniformMatrix4fv(_modelUV, 1, GL_FALSE, glm::value_ptr(_model));


	glm::mat4 view = _mouseStates[GLUT_RIGHT_BUTTON]._transform * _translate *
			         _mouseStates[GLUT_MIDDLE_BUTTON]._transform * _scale *
			         _mouseStates[GLUT_LEFT_BUTTON]._transform * _rotate;

	glUniformMatrix4fv(_viewUV, 1, GL_FALSE, glm::value_ptr(view));

	// Perspective
	float fov = 30.0f;
	float zNear = OBJECT_DEPTH - OBJECT_B_RAD;
	float zFar = OBJECT_DEPTH + OBJECT_B_RAD;
	if (_projectionMode == PERSPECTIVE)
	{
		_projection = glm::perspectiveFov((float)(fov * 2 * M_PI / 360.0f), _width, _height, zNear, zFar);
	}
	else
	{
		_projection = glm::ortho(-1/sinf(fov / 2), 1/sinf(fov / 2), -1/sinf(fov / 2), 1/sinf(fov / 2), zNear, zFar);
	}
	_projection = _projection * glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-OBJECT_DEPTH));

	glUniformMatrix4fv(_projectionUV, 1, GL_FALSE, glm::value_ptr(_projection));

	// Draw using the state stored in the Vertex Array object:
	glUniform1i(_isArcballUV, 0);
	glBindVertexArray(_vao[0]);
	glDrawElements(GL_TRIANGLES, _elementIndices.size(), GL_UNSIGNED_INT, &_elementIndices[0]);

	// Draw arcball circle
	glBindVertexArray(_vao[1]);
	glUniform1i(_isArcballUV, 1);
	glUniformMatrix4fv(_modelUV, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(_viewUV, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(_projectionUV, 1, GL_FALSE,
			glm::value_ptr(glm::scale(glm::mat4(1.0f), glm::vec3(1, _width / _height, 1))));

	glDrawArrays(GL_LINE_LOOP, 0, ARCBALL_VERTICES);
	glBindVertexArray(0);

	// Cleanup, not strictly necessary
	glUseProgram(0);
}

void Model::resize(int width, int height)
{
    _width	= width;
    _height = height;
}

glm::vec3 Model::get_normalized_location(int x, int y)
{
	glm::vec3 point(0,0,0);
	point.x = ((2*x / _width) - 1);
	point.y = ((_height - 2*y) / _height);
	float zSqr = pow(ARCBALL_RAD,2) - pow(point.x,2) - pow(point.y, 2);
	if (zSqr > 0)
	{
		point.z = sqrt(zSqr);
	}
	else
	{
		point.z = 0;
	}

	return point;
}

void Model::mouse_click(int button, bool isBegin, int x, int y)
{
	if (button < 0 || button > 2)
	{
		return;
	}

	MouseClickState& mouse_state = _mouseStates[button];
	mouse_state._isActive = isBegin;
	mouse_state._initialMouseLocation = get_normalized_location(x, y);
	if (! isBegin)
	{
		switch (button)
		{
		case GLUT_RIGHT_BUTTON:
			_translate = _translate * mouse_state._transform;
			break;
		case GLUT_MIDDLE_BUTTON:
			_scale = _scale * mouse_state._transform;
			break;
		case GLUT_LEFT_BUTTON:
			_rotate = mouse_state._transform * _rotate;
			break;
		}
		mouse_state._transform = glm::mat4(1.0f);
	}
}

void Model::mouse_move(int x, int y)
{
	glm::vec3 mouseLocation = get_normalized_location(x,y);
	for (size_t i = 0; i < _mouseStates.size(); ++i)
	{
		MouseClickState& state = _mouseStates[i];
		if (state._isActive)
		{
			switch (i)
			{
			case GLUT_RIGHT_BUTTON:
				state._transform = glm::translate(glm::mat4(1.0f),
						glm::vec3(mouseLocation.x - state._initialMouseLocation.x,
								  mouseLocation.y - state._initialMouseLocation.y,
								  0));
				break;
			case GLUT_MIDDLE_BUTTON:
				state._transform = glm::scale(glm::mat4(1.0f),
						glm::vec3(exp(state._initialMouseLocation.y - mouseLocation.y)));
				break;
			case GLUT_LEFT_BUTTON:
				if (mouseLocation.z != 0)
				{
					glm::vec3 rotation_axis = glm::cross(state._initialMouseLocation, mouseLocation);
					float dot = glm::dot(glm::normalize(state._initialMouseLocation),
							             glm::normalize(mouseLocation));
					float rotation_angle = acos(dot) * 2;
					state._transform = glm::rotate(glm::mat4(1.0f),rotation_angle, rotation_axis);
				}
				break;
			}
		}
	}
}

void Model::reset()
{
	_scale = glm::mat4(1.0f);
	_translate = glm::mat4(1.0f);
	_rotate = glm::mat4(1.0f);
}

void Model::switchPolygonMode()
{
	_polygonMode = (_polygonMode == GL_FILL) ? GL_LINE : GL_FILL;
}

void Model::switchPerspective()
{
	_projectionMode = (_projectionMode == PERSPECTIVE) ? ORTHO : PERSPECTIVE;
}

Model::MouseClickState::MouseClickState() :
_isActive(false),
_transform(1.0f),
_initialMouseLocation(0.0f)
{
}
