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
	float screen_right_edge = sqrt(width_height_ratio);
	float screen_left_edge = -screen_right_edge;
	float screen_top_edge = sqrt(1/width_height_ratio);
	float screen_bottom_edge = -screen_top_edge;

	float ball_right_edge = _position.x + _cur_radius;
	float ball_left_edge = _position.x - _cur_radius;
	float ball_top_edge = _position.y + _cur_radius;
	float ball_bottom_edge = _position.y - _cur_radius;

	if ((ball_right_edge >= screen_right_edge && _direction.x > 0) ||
		(ball_left_edge <= screen_left_edge && _direction.x < 0))
	{
		_direction = glm::reflect(_direction, glm::vec4(1,0,0,0));
	}
	if ((ball_top_edge >= screen_top_edge && _direction.y > 0) ||
		(ball_bottom_edge <= screen_bottom_edge && _direction.y < 0))
	{
		_direction = glm::reflect(_direction, glm::vec4(0,1,0,0));
	}

	// Move
	_position.x += _direction.x * step_size;
	_position.y += _direction.y * step_size;
	// Assert ball in screen
	_position.x = std::max(std::min(_position.x, screen_right_edge - _cur_radius), screen_left_edge + _cur_radius);
	_position.y = std::max(std::min(_position.y, screen_top_edge - _cur_radius), screen_bottom_edge + _cur_radius);
}
