200170694 orensam
200571396 orimo


=== Files ===
MyMesh.h               - Open mesh defs file
MyMeshObject.cpp       - A mesh object for the ray tracer
MyMeshObject.h         - A mesh object for the ray tracer
MyVecs.h               - More open mesh defs
Scene1.h               - Example scene 1
Scene2.h               - Example scene 2
Scene3.h               - Example scene 3
Scene4.h               - Our example scene - Santa and his ellipsoidal lens
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
Makefile			   - Compiles the program
textures/			   - Textures folder
meshes/			       - Meshes folder
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
Samples per pixels: 10
Number of reflected/refracted rays: 5
Cutoff angle: 3

In the command line:
ex5 10 5 3 0

In this scene, the reflections in the 'floor' have a big influence on the 'feel' of the resulting
image. Reflecting 5 rays every time, using a cutoff angle of 3 degrees seemed to yield the best results.
The 5 reflecting rays gave the floor reflection a realistic, 'blurred' feeling, and the the 3 degree
cutoff is as large as it can be without creating unrealistic artifacts.
Using 10 samples per pixels helped give a more correct coloring, and prevented some strings from disappearing,
a problem which frequently occurred when using less samples (very noticeable in the middle string). This number
of samples resulted in a reasonable runtime of less than 30 seconds on school lab computers.
 
 
==== Scene 2 parameters ====
Samples per pixels: 9
Number of reflected/refracted rays: 4
Cutoff angle: 1.7

In the command line:
ex5 9 4 1.7 1

In scene 2, there's a very obvious tradeoff between the number of refracted rays and the number of samples.
Since we have a spherical lens in the middle of the screen, the number of rays grows very quickly when
the refraction factor increases. Playing with the different parameters yielded an image rendered in about
one minute with the aforementioned parameters - Both a good averaging using 9 samples per pixels, and a good
utilization of the lens using a refraction factor of 4.
The cutoff angle was set to 1.7 after trial and error - the range of 1.4..1.9 is where there's a noticeable
effect of the angle, without totally losing the 'warning' texture in the ball reflection seen through the lens.   


==== Scene 3 parameters ====
Samples per pixels: 11
Number of reflected/refracted rays: 1
Cutoff angle: 1

In the command line:
ex5 11 1 1 2

In this scene, there aren't any visible reflections or refractions - the only reflective surface is
the floor, and it seems that it's either too far away, or not oriented in appropriately, to reflect
the sphere and the face mesh. So, if we want to have a meaningful effect we're pretty much left
with the samples-per-pixel parameter.
The fact that we have a mesh in this already has an adverse effect on the rendering time,
so we can only increase the sample number so much. 11 samples was the highest we could go
on the school lab computers for a rendering that takes under a minute.


=== Scene 4 - custom scene ===
Samples per pixels: 8
Number of reflected/refracted rays: 2
Cutoff angle: 0.6

In the command line:
ex5 8 2 0.6 3

This scene demonstrates most of what we implemented in this exercise:
Santa's body is an ellipsoid with a coat texture. 
The lens demonstrates refraction in an ellipsoid.
The trees, Santa's hat and Santa's legs are all cones.
The trees and floor demonstrate reflections.
The background is a plane with a mountain texture.
The floor is a reflective surface with a snow texture (might be a bit hard to see
because of the mountains reflecting in it).

The small cutoff angle keeps the lens refraction sharp, while making it reslisticly blurred.
The relatively large number of samples per pixels gave a better result than enlarging the
number of reflected/refracted rays, while keeping the runtime under a minute.


=== Quadrics ===

1. Ellipsoid - implemented as suggested in the ray tracing presentation from the tirgul -
a sphere with a 4d transformation. This resulted in a very simple and elegant code for
the Ellipsoid class.

2. Cone - this is our additional parametric shape. A cone is built according to an apex point
(the 'tip'), an axis (along which the cone is oriented) and a height.
Since a parametric cone is infinite on both sides, we 'cut' the cone with respect to two paralell
planes, perpendicular to the axis, and add a 'cap' at the bottom.  

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
4. Figuring out how to use the transformation T of the ellipsoid - it took a lot of trial and error
   to understand that we need to use T's inverse on the ray, T itself in the resulting point (from
   Sphere::intersect), and T's transpose on the resulting normal.


=== Web pages used ===
https://sites.google.com/site/justinscsstuff/object-intersection
http://www.starkeffects.com/snells-law-vector.shtml
http://mrl.nyu.edu/~dzorin/rend05/lecture2.pdf
http://www.geometrictools.com/Documentation/IntersectionLineCone.pdf
