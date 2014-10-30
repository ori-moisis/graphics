#ifndef __ex0__Ball__
#define __ex0__Ball__


class Ball
{
public:
	float _x;
	float _y;
	float _direction;
	float _cur_radius;
	float _max_radius;
	float _red, _green, _blue;

	Ball(float x, float y, float direction, float max_radius, float red, float green, float blue);

	/**
	 * Move the ball according to its current direction
	 */
	void move(float step_size);
};

#endif // __ex0__Ball__
