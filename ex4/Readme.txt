200571396 orimo
200170694 orensam

=== Files ===
ex4.cpp                    - Main program
Model.h                    - Header file for the Model class
Model.cpp                  - All OpenGL code
shaders/Arcball.vert       - A shader for the arcball - uniform color and only apply scale 
                             transformation
shaders/SimpleShader.vert  - A shader that applies the current transformation
							 and chooses the vertex color according to position.
shaders/SimpleShader.frag  - A shader that colors a fragment according to the interpolated values.
shaders/GouraudShader.vert - Shader that chooses vertex color according to Gouraud shading
shaders/Phong.vert         - Shader that applies the current transformation on vertex positions
                             and passes them along with the normal to the fragment shader
shaders/Phong.frag         - Shader that chooses fragment color according to Phong shading using
                             the position and normal interpolated from the phong vertex shader.
                             Now with textures
ShaderIO.h                 - Header file for the programManager class
ShaderIO.cpp               - OpenGL Shader interfacing code
Makefile				   - Compiles the program 
Readme.txt                 - This file


=== Implementation notes ===
Changes to the host program:
1. In the init method we now load the 3 texture files using the bimage library
2. Added support for texture control keys

Changes to Phong.frag:
1. Added a uniform variable that chooses the texture mode
2. Added uniform variables for the texture/bump map/scale/turbulence coefficient
3. Added the turbulence function as-is from the exercise definition.

Each texture mode may change the specular and/or diffuse coefficients to apply the texture to
the object.
There is also a new shading component called textureColor which the mirror texture uses in order
to be independent of the light sources.
Both marble and wood textures use the formulas given in the tirgul slides.
One notable difference is that we used sin^3 instead of sin in the marble texture, this was done
in order to have gray regions similarly to the school solution.


=== Configuration variables (same as ex3) === 

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
