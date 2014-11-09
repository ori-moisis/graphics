#include "Ball.h"

#include <math.h>
#include <algorithm>

Ball::Ball(size_t id, float x, float y, float direction, float max_radius, float red, float green, float blue)
: _id(id),
  _position(x,y,0,1),
  _direction(cosf(direction), sinf(direction), 0, 0),
  _cur_radius(max_radius),
  _max_radius(max_radius),
  _red(red),
  _green(green),
  _blue(blue)
{
}

void Ball::move(float step_size, float width_height_ratio)
{
	// Check for wall collision
	float x_radius = _cur_radius * std::min(1.0f, 1 / width_height_ratio);
	float right_edge = _position.x + x_radius;
	float left_edge = _position.x - x_radius;
	float y_radius = _cur_radius * std::min(1.0f, width_height_ratio);
	float top_edge = _position.y + y_radius;
	float bottom_edge = _position.y - y_radius;

	if ((right_edge >= 1 && _direction.x > 0) ||
		(left_edge <= -1 && _direction.x < 0))
	{
		_direction = glm::reflect(_direction, glm::vec4(1,0,0,0));
	}
	if ((top_edge >= 1 && _direction.y > 0) ||
		(bottom_edge <= -1 && _direction.y < 0))
	{
		_direction = glm::reflect(_direction, glm::vec4(0,1,0,0));
	}

	// Move
	_position.x += _direction.x * step_size * std::min(1.0f, 1 / width_height_ratio);
	_position.y += _direction.y * step_size * std::min(1.0f, width_height_ratio);
	_position.x = std::max(std::min(_position.x, 1 - x_radius), x_radius - 1);
	_position.y = std::max(std::min(_position.y, 1 - y_radius), y_radius - 1);
}
