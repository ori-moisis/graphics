//
//  scene.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : A class representing a scene, implementing the ray tracing
//            algorithm for color calculation of every ray in the scene.
//

#ifndef _SCENE_HH
#define _SCENE_HH


//////////////////////////////
// Library Includes         //
//////////////////////////////

#ifndef WIN32
#include "bimage.h"
#else
#include "bimage.h"
#endif
#include <vector>


//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"
#include "object.h"
#include "ray.h"
#include "lights.h"


//////////////////////////////
// Constants                //
//////////////////////////////

static const double MINIMAL_VIS       = 0.1;
static const double RECURSION_FACTOR  = 0.70;

//////////////////////////////
// Class Decleration        //
//////////////////////////////

using std::vector;

class Camera;

class Scene 
{
public:
  // Constructing a 3D scene
  Scene();
  Scene(Color3d & color, AmbientLight & light, double cutoffAngle);

  // trace_ray - Ray scene intersection for color rays
  Color3d trace_ray(Ray ray, double vis = 1.0) const;

  // add_object - add the given object to the scene
  void add_object(Object * obj);

  // add_light - add the given light to the scene
  void add_light(PointLight * light);

  // backgroundColor - return reference to the background color
  Color3d & backgroundColor() {return _background;}

  // ambientLight - return reference to the ambient light
  AmbientLight & ambientLight() {return _ambientLight;}

	// Returns a ray in a cone with cutoff _cutoff around the input ray r
  Ray perturbateRay(const Ray& r) const;
	
  void setCutoffAngle(double cutoff)
	{
		_cutoffAngle = cutoff;
	}
	
  void setNumberOfRefRays(size_t n)
	{
		_numberOfRefRays = n;
	}
	
  virtual void setDefaultCamera(Camera& camer) const = 0;
	
  virtual ~Scene() {

  }
	
private:
  // find the nearest object intersecting with the given ray
  bool findNearestObject(IN Ray ray, OUT Object** object, OUT double& t, OUT Point3d& P, OUT Vector3d& N, OUT Color3d& texColor) const;
  
  // calculate the reflection color at the given intersection point
  Color3d calcReflection(const Ray& ray, const Point3d& P, const Vector3d& N, const Object& object, double vis = 1.0, bool isCritical = false) const;

  // calculate the refraction color at the given intersection point
  Color3d calcRefraction(const Ray& ray, const Point3d& P, const Vector3d& N, const Object& object, double vis = 1.0) const;

private:
  
  vector<Object *>        _objects;       // The scene's objects          //
  vector<PointLight * >   _lights;        // The scene's point lights     //

  AmbientLight            _ambientLight;  // The scene's Ambient light    //
  Color3d                 _background ;   // The scene's background color //

  Color3d                 _lastReflection;  // The last calculated reflection color //
  
  double _cutoffAngle;
  double _numberOfRefRays;
};

#endif /* _SCENE_HH */
