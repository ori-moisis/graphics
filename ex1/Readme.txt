200571396 orimo
200170694 orensam

=== Files ===
ex0.h                     - Main program header file
ex0.cpp                   - Main program
Model.h                   - Header file for the Model class
Model.cpp                 - All OpenGL code for drawing a circle
shaders/SimpleShader.vert - A shader that changes the radius of a 2D circle
shaders/SimpleShader.frag - A shader that colors a fragment in a checkerboard pattern


--- ex1a files ---

ex1a.cpp                    - Main program header file
ex1a.h                      - Main program  
ModelA.cpp                  - All OpenGL code for drawing a circle
ModelA.h                    - Model header
shadersA/SimpleShader.frag  - Shader that colors a fragment in a checkerboard pattern
shadersA/SimpleShader.vert  - Shader that changes the radius of a 2D circle

--- ex1b files ---
Ball.cpp                    - Class that represents a ball.
Ball.h                      - Header for ball class
ex1b.cpp                    - Main ex1b program
ex1b.h                      - ex1b header
ModelB.cpp                  - All OpenGL code for the bouncy balls
ModelB.h                    - Model header
shadersB/SimpleShader.frag  - Shader for coloring and lighting the balls
shadersB/SimpleShader.vert  - Shader for applying transformations on vertices

--- Common files ---
ShaderIO.h                - Header file for the programManager class
ShaderIO.cpp              - OpenGL Shader interfacing code
Readme.txt                - This file


=== Implementation notes ===
In ex1a we drew a single circle with a relativly simply fragment shader.
For ex1b we kept the basic circle vertex array and allowed the vertex shader to draw many transformed
circles with the same basic vertex array.
Our high level implementation consists of a vector of balls, each with a position, color, current
direction, current radius and maximum radius. 
The color and initial direction are randomly chosen when the ball is created, the maximum radius is
determined according to the distance of the mouse click from the walls and any surrounding balls.

The host application is responsible for moving the balls and the lighting source, as well as 
calculating any changes to the balls radiuses due to ball-to-ball collisions.
The host application also calculates the transformation matrix for each ball (using glm).
The vertex shader is quite simple - it just applies the relevant transformation on each ball's 
vertices.
The fragment shader is given the location of the light source and, using the ball's transformation,
calculates the position and gradiant of the lighting effect on the ball.

In order to minimize the amount of data transferred between the host application and the shaders we:
1. Use the same transformation for calculating both the ball's location and all lighting effects
2. Use glDrawArraysInstanced in order to draw up to 32 balls at once.

Note that we did not have to change the z coordinate of the balls because according to the exercise
specification they should never overlap.
Also note - we implemeted a rotating light source

=== Configuration variables ===
1. 
In order to maintain smooth circles regardless of resolution we calculate the number of vertices
in a circle with the following formula:
max((_width * _height) / VERTEX_RATIO, MIN_VERTICES)
With default values:
MIN_VERTICES = 50;
VERTEX_RATIO = 5000;

2.
Ball related constants:
// Maximum and minimum radius for a ball
static const float BASE_RADIUS = 0.1;
static const float MIN_RADIUS = 0;
// Speed of the balls
static const float STEP_SIZE = 0.02;

3.
Light source
// Rotation speed of the light source
static const float LIGHT_SPEED = 0.02;

4. 
// Number of balls to draw in each call to glDrawArraysInstanced
static const int M = 32;


=== Data flow between host and shaders ===
In each draw iteration the host sets the following uniform variables for the shaders

1. Current perspective matrix
2. The transformation matrix for each ball
3. The color of each ball
4. The current location of the light source (used by the fragment shader to create the lighting)

=== Non-trivial fragment shader code ===
In order to keep data transfer between the host and the shader to a minimum, some constants were
defined directly in the fragment shader:

1. The light color is defined to be white
2. The light fade factor - determines how fast the light intensity decreases with distance from
   the light center
3. lightRadius - The cutoff radius of the lighting effect

The lighting calculation is done in the following way:
1. Figure out where on the screen the light center is (this takes the original light center 
   location from the host and moves it along with the ball)
2. Calculate the distance of the current pixel from the light center
3. According to that distance, the fade factor and the radius, decide how to mix the ball
   color with the light color 
