200571396 orimo
200170694 orensam

=== Files ===
ex3.cpp                    - Main program
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
                             the position and normal interpolated from the phong vertex shader
ShaderIO.h                 - Header file for the programManager class
ShaderIO.cpp               - OpenGL Shader interfacing code
Makefile				   - Compiles the program 
Readme.txt                 - This file


=== Implementation notes ===
We have 3 different VAO/VBOs:
1. The arcball vertices
2. The mesh vertices and normals with each vertex appearing once
3. The mesh vertices and normals with each vertex appearing multiple times (one for every face
   the vertex is a part of)

The reason for having two VAO/VBOs for the mesh is to minimize the amount of data being transferred
in large meshes: if we only had one VAO/VBO for the mesh, we would have to pass both the basic
and advanced normals for each vertex. 
since each vertex usually appears in more than 2 faces, this would mean passing the same advanced
normal multiple times.
By using two separate VAO/VBOs we allow the advanced normal mode to pass each normal just once
and thus pass less data between our application and the shader.

We have 4 OpenGL programs:
  Program    |  Vertex shader     | Fragment shader
---------------------------------------------------
1. Phong     | Phong.vert         | Phong.frag
2. Gouraud   | GouraudShader.vert | SimpleShader.frag
3. Colorful  | SimpleShader.vert  | SimpleShader.frag
4. Arcball   | Arcball.vert       | SimpleShader.frag

Note that the gouraud vertex shader and the phong fragment shader perform pretty much the same
calculation, the only difference is the phong shader does so with interpolated normals at each
pixel and the gouraud shader does so only for each vertex.


=== Configuration variables (same as ex2) === 

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
