200571396 orimo
200170694 orensam

=== Files ===
ex2.h                     - Main program header file
ex2.cpp                   - Main program
Model.h                   - Header file for the Model class
Model.cpp                 - All OpenGL code for drawing a circle
shaders/SimpleShader.vert - A shader that applies the current transformation
							and chooses the vertex color.
shaders/SimpleShader.frag - A shader that colors a fragment according to the interpolated values.
ShaderIO.h                - Header file for the programManager class
ShaderIO.cpp              - OpenGL Shader interfacing code
Readme.txt                - This file


=== Implementation notes ===
In this exercise, we display a given OpenMesh mesh using the tools OpenGL provides us,
and allow the user to perform transformations on the model using the keyboard and mouse.

When the mesh is read, we calculate the bounding box and center of the given model, 
and use these parameters in order to create the model transformation, which normalizes the model
such that its longest dimension is stretched or compressed to span [-1, 1].

Upon loading the mesh, we iterate the model's faces and create element indices to be used with
glDrawElements.

We use two separate VAOs/VBOs: One for the displayed model, and another for the arcball, which
is represented on the screen simply as a red circle surrounding the model's initial location.

The model's colors are chosen in the vertex shader, according to its initial normalized 
coordinates. The fragments are then colored by an interpolation of the colors chosen for 
the surrounding vertices.

Two additional important transformations which we apply to our model are 
the view and projection transformations.

The view matrix is responsible for the rotation and translation of the model
(in accordance with the user's mouse input). This matrix is calculated each time 
in the draw() function, according to the most recent values available. 

The projection matrix is responsible for the perspective and zoom. It too must be calculated
in each draw(), since it is calculated according to the current zoom level, and is then 
combined with the current projection type (either perspective or orthogonal)

The matrices are applied to the model, in the vertex shader, in the following order:
Model (stretches the model and puts the center at (0,0,0))
View (First rotates the model around (0,0,0) - i.e the new model center, and then translates 
the model to the requested location)
Projection (Applies the current projection (perspective/ortho) to the model)    
  

=== Configuration variables === 

1. ARCBALL_VERTICES = 100
Number of vertices in the arcball

2. ARCBALL_RAD = 0.8
Arcball radius

3. OBJECT_DEPTH = 7.6
Depth of the model

4. OBJECT_B_RAD = 4.0
Clipping plane distance from object

5. FOV = 30.0
Initial field of view (degrees)

6. MIN_FOV = 0.1
7. MAX_FOV = M_PI - 0.35
FOV limits (radians)


=== Data flow between host and shaders ===
In each draw iteration the host sets the following uniform variables for the shaders:

1. Current model, view, projection matrices.
2. A boolean denoting whether the current vertex belongs to the model or the arcball.
