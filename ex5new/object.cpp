#include "object.h"

Object::Object()
: _ambient(COLOR_DEFAULT_AMBIENT)
, _diffuse(COLOR_BLACK)
, _specular(COLOR_BLACK)
, _reflection(COLOR_BLACK)
, _transparency(COLOR_BLACK)
, _shining (DEFAULT_SHINING)
, _index (DEFAULT_INDEX)
, _diffuseTexture (0) {
}

void Object::set_texture_map(BImage* image) {
	this->_diffuseTexture = image;
}

Color3d& Object::diffuse() {
	return this->_diffuse;
}

Color3d& Object::specular() {
	return this->_specular;
}

Color3d& Object::ambient() {
	return this->_ambient;
}

Color3d& Object::reflection() {
	return this->_reflection;
}

Color3d& Object::transparency() {
	return this->_transparency;
}

double& Object::index() {
	return this->_index;
}

double& Object::shining() {
	return this->_shining;
}

const Color3d& Object::getDiffuse() const {
	return this->_diffuse;
}

const Color3d& Object::getSpecular() const {
	return this->_specular;
}

const Color3d& Object::getAmbient() const {
	return this->_ambient;
}

const Color3d& Object::getReflection() const {
	return this->_reflection;
}

const Color3d& Object::getTransparency() const {
	return this->_transparency;
}

const double Object::getIndex() const {
	return this->_index;
}

const double Object::getShining() const {
	return this->_shining;
}

