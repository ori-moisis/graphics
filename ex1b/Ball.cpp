#include "Ball.h"

#include <math.h>

Ball::Ball(float x, float y, float direction, float max_radius, float red, float green, float blue)
: _x (x),
  _y(y),
  _direction(direction),
  _cur_radius(max_radius),
  _max_radius(max_radius),
  _red(red),
  _green(green),
  _blue(blue)
{
}

void Ball::move(float step_size)
{
	// Check for wall collision
	if (_x + _cur_radius >= 1 ||
		_x - _cur_radius <= -1)
	{
		_direction = M_PI - _direction;
	}
	if (_y + _cur_radius >= 1 ||
		_y - _cur_radius <= -1)
	{
		_direction = (2*M_PI) - _direction;
	}

	_x += cosf(_direction) * step_size;
	_y += sinf(_direction) * step_size;
}
