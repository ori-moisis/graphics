#ifndef __ex0__Ball__
#define __ex0__Ball__

#include <glm/glm.hpp>
#include <vector>

class Ball
{
public:
	size_t _id;
	glm::vec4 _position;
	glm::vec4 _direction;
	float _cur_radius;
	float _max_radius;
	float _red, _green, _blue;

	Ball(size_t id, float x, float y, float direction, float max_radius, float red, float green, float blue);

	/**
	 * Move the ball according to its current direction
	 */
	void move(float step_size, float width_height_ratio);
};

#endif // __ex0__Ball__
