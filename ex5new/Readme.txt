200571396 orimo
200170694 orensam

=== Files ===
MyMesh.h               - Open mesh defs file
MyMeshObject.cpp       - A mesh object for the ray tracer
MyMeshObject.h         - A mesh object for the ray tracer
MyVecs.h               - More open mesh defs
Scene1.h               - Example scene 1
Scene2.h               - Example scene 2
Scene3.h               - Example scene 3
Scene4.h               - Our example scene
SceneChooser.h         - The scene chooser
bimage.cpp             - The bimage library
bimage.h               - The bimage library
camera.cpp             - The camera
camera.h               - The camera
cone.cpp               - A cone object for the ray tracer (our parametric object)
cone.h                 - A cone object for the ray tracer (our parametric object)
ellipsoid.cpp          - An ellipsoid object for the ray tracer 
ellipsoid.h            - An ellipsoid object for the ray tracer
ex5.cpp                - The main file
general.h              - General definitions
lights.h               - Light defenitions (including the sphere light class)
object.cpp             - Base class for all ray tracer objects
object.h               - Base class for all ray tracer objects
polygon.cpp            - A polygon object for the ray tracer
polygon.h              - A polygon object for the ray tracer
ray.h                  - A single ray
scene.cpp              - A scene that holds all objects and traces the rays
scene.h                - A scene that holds all objects and traces the rays
sphere.cpp             - A sphere object for the ray tracer
sphere.h               - A sphere object for the ray tracer
triangle.cpp           - A triangle - polygons use the triangle in order to determine if and where
                         an intersection with a ray occurs 
triangle.h             - A triangle
Readme.txt             - This file



=== Implementation notes ===
==== Sphere light implementation ====
Our sphere light is actually a regular sphere, but instead of adding it as an object we add it
to a separate "sphere lights" list.
Whenever we have to check the amount of light an object sees from a sphere light source, we simply
shoot a ray from that object to a randomized location on the sphere and check whether the light
is the nearest object in that direction (in order to do that, we use the same code in
findNearestObject with an additional object (the sphere light) which is checked first (in order to
limit the "t" to the correct value).
By shooting multiple rays to the sphere and checking whether the sphere is the nearest object we
determine the "shadow ratio" which is used to calculate the amount of light that reaches the object

==== Optimizations ====
When we ran our code through a profiler we saw that a lot of time was spent in triangle's isInside
function. 
We optimized that function by calculating once everything we could calculate in advance, which
seemed to help.
We also noticed that calls to mesh.faces_end are not optimized away by the compiler like calls
to vector.end are, getting faces_end before each loop seemed to help a lot for scenes with meshes

==== Scene 1 parameters ====

Explanations:

==== Scene 2 parameters ====

Explanations:

==== Scene 3 parameters ====

Explanations:


==== The hard parts ====
1. In the beginning of the exercise, in camera class we had two difficult parts:
1.1. Figuring out how to transform the scene to a normalized coordinate system
1.2. Figuring out how to calculate the directions of the ray we have to shoot through each pixel.
     This was diffictult mainly because the provided slides gave us a formula that did not really
     work (it took us quite a while to realize which part was missing from the tirgul slides)   
2. When we implemented refractions, it was pretty difficult getting the vector version of snell's 
   law to work, in hindsight we are not really sure why it did not work for the first 3 hours we
   tried it, but it was still difficult.
   We also spent quite a lot of time battling normal directions in this context.  
3. When we implemented textures it was really hard to understand which coordinates we were supposed
   to use, mainly for polygons since the instructions were very general and short.


=== Web pages used ===
https://sites.google.com/site/justinscsstuff/object-intersection
http://www.starkeffects.com/snells-law-vector.shtml