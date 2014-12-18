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

#include <bimage.h>

#define SHADERS_DIR "shaders/"
#define TEXTURE_DIR "textures/"
#define MIRROR_TEX_FILE "spheremap2.bmp"
#define BRICK_TEX_FILE "brickwork-texture.bmp"
#define BRICK_BUMP_FILE "brickwork-bump-map.bmp"

// Number of vertices in the arcball
static const int ARCBALL_VERTICES = 100;
// arcball radius
static const float ARCBALL_RAD = 0.8f;

// Depth of the mesh
static const float OBJECT_DEPTH = 7.6f;
// clipping plane distance from object
static const float OBJECT_B_RAD = 4.0f;
// FOV (degrees)
static const float FOV = 30.0f;
// FOV limits (radians)
static const float MIN_FOV = 0.1f;
static const float MAX_FOV = M_PI - 0.35f;


Model::Model() :
_polygonMode(GL_FILL),
_fov(static_cast<float>(FOV * M_PI / 180.0f)),
_translate(1.0f),
_rotate(1.0f),
_fovChange(0.0f),
_shininess(200),
_texScale(4),
_turbCoeff(6),
_texMode(MARBLE),
_mouseStates(3, MouseClickState()),
_projectionMode (PERSPECTIVE),
_lightingMode (PHONG),
_normalMode (ADVANCED)
{
	memset(_vao, 0, sizeof(_vao));
	memset(_vbo, 0, sizeof(_vbo));
}

Model::~Model()
{
	if (_vao[0] != 0)
		glDeleteVertexArrays(3, _vao);
	if (_vbo[0] != 0)
		glDeleteBuffers(3, _vbo);
}

bool Model::init(const std::string& mesh_filename)
{
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	programManager::sharedInstance()
			.createProgram("phong",
						   SHADERS_DIR "Phong.vert",
						   SHADERS_DIR "Phong.frag");

	programManager::sharedInstance()
			.createProgram("gouraud",
						   SHADERS_DIR "GouraudShader.vert",
						   SHADERS_DIR "SimpleShader.frag");

	programManager::sharedInstance()
		.createProgram("default",
					   SHADERS_DIR "SimpleShader.vert",
					   SHADERS_DIR "SimpleShader.frag");

	programManager::sharedInstance()
		.createProgram("arcball",
					   SHADERS_DIR "Arcball.vert",
					   SHADERS_DIR "SimpleShader.frag");


	_programs[0] = programManager::sharedInstance().programWithID("phong");
	_programs[1] = programManager::sharedInstance().programWithID("gouraud");
	_programs[2] = programManager::sharedInstance().programWithID("default");
	_programs[3] = programManager::sharedInstance().programWithID("arcball");



	// Obtain uniform variable handles:
	for (int i = 0; i < 4; ++i)
	{
		_modelUV[i] = glGetUniformLocation(_programs[i], "model");
		_viewUV[i] = glGetUniformLocation(_programs[i], "view");
		_projectionUV[i] = glGetUniformLocation(_programs[i], "projection");
		if (i < 2)
		{
			_shininessUV[i] = glGetUniformLocation(_programs[i], "shininess");
		}
		if (i == PHONG)
		{
			_texScaleUV = glGetUniformLocation(_programs[i], "texScale");
			_turbCoeffUV = glGetUniformLocation(_programs[i], "turbCoeff");
			_texModeUV = glGetUniformLocation(_programs[i], "texMode");
			_textureUV = glGetUniformLocation(_programs[i], "myTexture");
			_bumpsUV = glGetUniformLocation(_programs[i], "myBumps");
		}
	}

	// Load mesh
	_mesh.request_vertex_normals();
	_mesh.request_face_normals();
	OpenMesh::IO::Options read_options;
	if (!OpenMesh::IO::read_mesh(_mesh, mesh_filename.c_str(), read_options))
	{
		// if we didn't make it, exit...
		return false;
	}
	if (! read_options.check(OpenMesh::IO::Options::VertexNormal))
	{
		_mesh.update_vertex_normals();
	}
	if (! read_options.check(OpenMesh::IO::Options::FaceNormal))
	{
		_mesh.update_face_normals();
	}

	// Create mesh vertices and bounding box
	const float max_float = std::numeric_limits<float>::max();
	Mesh::Point lowerLeft(max_float, max_float, max_float);
	Mesh::Point upperRight(-max_float, -max_float, -max_float);
	Mesh::Point center(0,0,0);

	std::vector<float> mesh_one_normal_per_vertex;
	mesh_one_normal_per_vertex.reserve(_mesh.n_vertices() * 8);
	int i = 0;
	for (Mesh::VertexIter iter = _mesh.vertices_begin();
		 iter != _mesh.vertices_end();
		 ++iter, ++i)
	{
		Mesh::Point p = _mesh.point(*iter);
		center += p;
		for (int j = 0; j < 3; ++j)
		{
			mesh_one_normal_per_vertex.push_back(p[j]);
			lowerLeft[j] = fmin(lowerLeft[j], p[j]);
			upperRight[j] = fmax(upperRight[j], p[j]);
		}
		mesh_one_normal_per_vertex.push_back(1);

		Mesh::Normal norm = _mesh.calc_vertex_normal(iter);
		for (int j = 0; j < 3; ++j)
		{
			mesh_one_normal_per_vertex.push_back(norm[j]);
		}
		mesh_one_normal_per_vertex.push_back(0);
	}
	center /= (double)_mesh.n_vertices();

	// Iter faces and add to element list
	std::vector<float> mesh_one_normal_per_face;
	mesh_one_normal_per_face.reserve(_mesh.n_faces() * 3 * 8);
	for (Mesh::FaceIter iter = _mesh.faces_begin();
		 iter != _mesh.faces_end();
		 ++iter)
	{
		Mesh::Normal norm = _mesh.calc_face_normal(*iter);

		Mesh::FaceVertexIter end_iter = _mesh.fv_end(*iter);
		for (Mesh::FaceVertexIter v_iter = _mesh.fv_begin(*iter); v_iter != end_iter; ++v_iter)
		{
			_elementIndices.push_back(v_iter.handle().idx());

			Mesh::Point p = _mesh.point(v_iter.handle());
			for (int j = 0; j < 3; ++j)
			{
				mesh_one_normal_per_face.push_back(p[j]);
			}
			mesh_one_normal_per_face.push_back(1);

			for (int j = 0; j < 3; ++j)
			{
				mesh_one_normal_per_face.push_back(norm[j]);
			}
			mesh_one_normal_per_face.push_back(0);
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
	glGenVertexArrays(3, _vao);
	glGenBuffers(3, _vbo);

	// Create and bind the mesh VAO with one normal per vertex
	glBindVertexArray(_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);

	for (int i = 0; i < 4; ++i)
	{
		GLint posAttrib = glGetAttribLocation(_programs[i], "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, // attribute handle
							  4,          // number of scalars per vertex
							  GL_FLOAT,   // scalar type
							  GL_FALSE,
							  sizeof(glm::vec4)*2,
							  0);

		GLint normalAttrib = glGetAttribLocation(_programs[i], "normal");
		glEnableVertexAttribArray(normalAttrib);
		glVertexAttribPointer(normalAttrib, // attribute handle
							  4,            // number of scalars per vertex
							  GL_FLOAT,     // scalar type
							  GL_FALSE,
							  sizeof(glm::vec4)*2,
							  (GLvoid*)(sizeof(glm::vec4)));
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, mesh_one_normal_per_vertex.size() * sizeof(float), &mesh_one_normal_per_vertex[0], GL_STATIC_DRAW);


    // Create and bind the mesh VAO with one normal per face
	glBindVertexArray(_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);

	for (int i = 0; i < 4; ++i)
	{
		GLint posAttrib = glGetAttribLocation(_programs[i], "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, // attribute handle
							  4,          // number of scalars per vertex
							  GL_FLOAT,   // scalar type
							  GL_FALSE,
							  sizeof(glm::vec4)*2,
							  0);

		GLint normalAttrib = glGetAttribLocation(_programs[i], "normal");
		glEnableVertexAttribArray(normalAttrib);
		glVertexAttribPointer(normalAttrib, // attribute handle
							  4,            // number of scalars per vertex
							  GL_FLOAT,     // scalar type
							  GL_FALSE,
							  sizeof(glm::vec4)*2,
							  (GLvoid*)(sizeof(glm::vec4)));
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, mesh_one_normal_per_face.size() * sizeof(float), &mesh_one_normal_per_face[0], GL_STATIC_DRAW);



   	// Bind arcball vertex array
   	glBindVertexArray(_vao[2]);
   	glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]);

   	for (int i = 0; i < 4; ++i)
	{
		GLint posAttrib = glGetAttribLocation(_programs[i], "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, // attribute handle
							  4,          // number of scalars per vertex
							  GL_FLOAT,   // scalar type
							  GL_FALSE,
							  0,
							  0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, ARCBALL_VERTICES * 4 * sizeof(float), arc_vertices, GL_STATIC_DRAW);

	// Unbind vertex array:
	glBindVertexArray(0);

    delete [] arc_vertices;


    // Load textures
    {
    	BImage image(TEXTURE_DIR MIRROR_TEX_FILE);

    	glGenTextures(1, &_mirrorTexUV);
    	glBindTexture(GL_TEXTURE_2D, _mirrorTexUV);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D,
        		     0 /* level */,
        		     GL_RGBA /* internalformat */,
        		     image.width(),
        		     image.height(),
        		     0 /* border*/,
        		     GL_RGB /* format */,
        		     GL_UNSIGNED_BYTE /* type */,
        		     image.getImageData());

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    {
    	BImage image(TEXTURE_DIR BRICK_TEX_FILE);

    	glGenTextures(1, &_bricksTexUV);
    	glBindTexture(GL_TEXTURE_2D, _bricksTexUV);

    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D,
					 0 /* level */,
					 GL_RGBA /* internalformat */,
					 image.width(),
					 image.height(),
					 0 /* border*/,
					 GL_RGB /* format */,
					 GL_UNSIGNED_BYTE /* type */,
					 image.getImageData());

		glBindTexture(GL_TEXTURE_2D, 0);
    }
    {
		BImage image(TEXTURE_DIR BRICK_BUMP_FILE);

		glGenTextures(1, &_bumpTexUV);
		glBindTexture(GL_TEXTURE_2D, _bumpTexUV);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D,
					 0 /* level */,
					 GL_RGBA /* internalformat */,
					 image.width(),
					 image.height(),
					 0 /* border*/,
					 GL_RGB /* format */,
					 GL_UNSIGNED_BYTE /* type */,
					 image.getImageData());

		glBindTexture(GL_TEXTURE_2D, 0);
	}

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
	glUseProgram(_programs[_lightingMode]);

	glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

	glUniformMatrix4fv(_modelUV[_lightingMode], 1, GL_FALSE, glm::value_ptr(_model));

	// View matrix: rotate, then translate (so rotation axis is preserved)
	glm::mat4 view = _mouseStates[GLUT_RIGHT_BUTTON]._transform * _translate *
			 	 	 _mouseStates[GLUT_LEFT_BUTTON]._transform * _rotate;

	// Move the object to the required depth as the last step before projection so the object
	// depth won't be affected by any other transformation
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-OBJECT_DEPTH)) * view;

	glUniformMatrix4fv(_viewUV[_lightingMode], 1, GL_FALSE, glm::value_ptr(view));

	// Perspective matrix: either projection, or orthogonal.
	// FOV changes according to mouse zoom.
	float fov = _fov + _fovChange;
	float zNear = OBJECT_DEPTH - OBJECT_B_RAD;
	float zFar = OBJECT_DEPTH + OBJECT_B_RAD;
	if (_projectionMode == PERSPECTIVE)
	{
		_projection = glm::perspective(fov, 1.0f, zNear, zFar);
	}
	else
	{
		float offsetFromCenter = OBJECT_DEPTH * tan(fov/2);
		_projection = glm::ortho(-offsetFromCenter, 
								 offsetFromCenter, 
								 -offsetFromCenter, 
								 offsetFromCenter, 
								 zNear, zFar);
	}

	// Manually fix aspect ratio so it will behave the same for perspective and ortho 
	// (and conform with school solution)
	glm::mat4 aspect = glm::scale(glm::mat4(1.0f),
								  glm::vec3(std::max(1.0f, _height / _width),
										  	std::max(1.0f, _width / _height),
										  	1.0f));
	_projection = aspect * _projection;

	glUniformMatrix4fv(_projectionUV[_lightingMode], 1, GL_FALSE, glm::value_ptr(_projection));

	if (_lightingMode == PHONG || _lightingMode == GOURAUD) {
		glUniform1i(_shininessUV[_lightingMode], _shininess);
	}
	if (_lightingMode == PHONG) {
		glUniform1i(_texScaleUV, _texScale);
		glUniform1i(_turbCoeffUV, _turbCoeff);
		glUniform1i(_texModeUV, _texMode);
		if (_texMode == MIRROR) {
			glUniform1i(_textureUV, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _mirrorTexUV);
		} else if (_texMode == BUMP) {
			glUniform1i(_textureUV, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _bricksTexUV);
			glUniform1i(_bumpsUV, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _bumpTexUV);
		}
	}

	switch (_normalMode)
	{
	case BASIC:
		glBindVertexArray(_vao[1]);
		glDrawArrays(GL_TRIANGLES, 0, _mesh.n_faces() * 3);
		break;
	case ADVANCED:
		glBindVertexArray(_vao[0]);
		glDrawElements(GL_TRIANGLES, _elementIndices.size(), GL_UNSIGNED_INT, &_elementIndices[0]);
		break;
	}

	// Draw arcball circle
	glUseProgram(_programs[ARCBALL]);
	glBindVertexArray(_vao[2]);
	glUniformMatrix4fv(_projectionUV[ARCBALL], 1, GL_FALSE, glm::value_ptr(aspect));

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
	point.x = ((2*x / _width) - 1) / std::max(1.0f, _height / _width);
	point.y = ((_height - 2*y) / _height) / std::max(1.0f, _width / _height);
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
			_fov = _fov + _fovChange;
			_fovChange = 0;
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
				_fovChange = std::max(-_fov + MIN_FOV,
						std::min(static_cast<float>(MAX_FOV - _fov),
								mouseLocation.y - state._initialMouseLocation.y));
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
	_fov = static_cast<float>(FOV * M_PI / 180.0f);
	_translate = glm::mat4(1.0f);
	_rotate = glm::mat4(1.0f);
	_shininess = 200;
	_texMode = MARBLE;
	_texScale = 4;
	_turbCoeff = 6;
}

void Model::switchPolygonMode()
{
	_polygonMode = (_polygonMode == GL_FILL) ? GL_LINE : GL_FILL;
}

void Model::switchPerspective()
{
	_projectionMode = (_projectionMode == PERSPECTIVE) ? ORTHO : PERSPECTIVE;
}

void Model::switchNormalMode()
{
	_normalMode = (_normalMode == BASIC) ? ADVANCED : BASIC;
}

void Model::setLightingMode(Model::LightingMode newMode)
{
	_lightingMode = newMode;
}

void Model::addShininess(int val)
{
	_shininess = std::min(2000, std::max(_shininess + val, 0));
}

void Model::addTexScale(int val)
{
	_texScale = std::min(INT_MAX, std::max(1, _texScale + val));
}

void Model::addTurbCoeff(int val)
{
	_turbCoeff = std::min(INT_MAX, std::max(0, _turbCoeff + val));
}

void Model::toggleTexMode()
{
	_texMode = static_cast<TexMode>((_texMode + 1) % NUM_TEX_MODES);
}

Model::MouseClickState::MouseClickState() :
_isActive(false),
_transform(1.0f),
_initialMouseLocation(0.0f)
{
}
