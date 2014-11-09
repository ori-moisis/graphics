200571396 orimo
200170694 orensam

=== Files ===
ex0.h                     - Main program header file
ex0.cpp                   - Main program
Model.h                   - Header file for the Model class
Model.cpp                 - All OpenGL code for drawing a circle
shaders/SimpleShader.vert - A shader that changes the radius of a 2D circle
shaders/SimpleShader.frag - A shader that colors a fragment in a checkerboard pattern
ShaderIO.h                - Header file for the programManager class
ShaderIO.cpp              - OpenGL Shader interfacing code
Readme.txt                - This file

=== Implementation notes ===
Our code is based on the original supplied code that draws an empty triangle.
Instead of a simple triangle, we approximated the shape of a circle using OpenGL's triangle fan.
The number of vertices that compose the circle's edge is linearly dependent on the 
current screen resolution, and has a lower bound of 50 vertices.
The formula to calculate the number of vertices is as follows:

max( width * height / VERTEX_RATIO, MIN_VERTICES);
(where VERTEX_RATIO is set in our code to 5000, and MIN_VERTICES to 50).
This means that, for example, when using a resolution of  1024x768, the number of 
vertices will be 157, which seems visually adequate.

We use the vertex shader in order to determine the radius of the circle and we use the fragment
shader in order to create the checkerboard pattern by setting the color of portions of the
fragment to black.
The following condition is used to determine whether a pixel will be colored by the fragment shader

(int(gl_FragCoord.x / squareSize) % 2) + (int(gl_FragCoord.y / squareSize) % 2) != 1

(where squareSize is set to 7 by the main program)