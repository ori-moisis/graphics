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

The The host application is resposible for moving the balls and the lighting source, as well as 
calculating any changes to the balls radiuses due to ball-to-ball collisions.
The host application also calculates the transformation matrix for each ball (using glm).
The vertex shader is quite simple - it just applies the relevant transformation on each ball's 
vertices.
The fragment shader is given the location of the light source and, using the ball's transformation,
calculates the position and gradiant of the lighting effect on the ball.

In order to minimize the amount of data transferred between the host application and the shaders we:
1. Use the same transformation for calculating both the ball's location and all lighting effects
2. Use glDrawArraysInstanced in order to draw 10 balls at once.

Note that we did not have to change the z coordinate of the balls because according to the exercise
specification they should never overlap.
Also note - we implemeted a rotating light source
