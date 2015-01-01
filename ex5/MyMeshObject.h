//
//  MyMeshObject.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : Decleration of an object, made out of a Mesh.
//

#ifndef __MY_MESH_OBJECT_
#define __MY_MESH_OBJECT_

//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"
#include "MyMesh.h"
#include "object.h"
#include "polygon.h"
#include "sphere.h"


//////////////////////////////
// Type defintions          //
//////////////////////////////

class Sphere;


//////////////////////////////
// Class Decleration        //
//////////////////////////////

class MyMeshObject : public Object
{
public:
  // Constructor - create a MyMeshObject from the given mesh  //
  MyMeshObject(MyMesh & mesh);

  // Destructor - free a MyMeshObject //
  virtual ~MyMeshObject();

  virtual void set_texture_map(BImage* image);

  // intersect - calculate intersection of a given ray with the object  //
  virtual int intersect(IN Ray& ray, IN double tMax, OUT double& t, OUT Point3d& P, OUT Vector3d& N, OUT Color3d& texColor);

private:
  // calculate and create a bounding sphere for the object  //
  void calculateBoundingSphere();

private:
  MyMesh    _mesh;            // The mesh the object is made of   //
  Sphere*   _boundingSphere;  // The sphere bounding the object   //
  Point3d   _center;          // The center of the mesh           //

  // Handle to a polygon pointer property for a MyMesh's face //
  OpenMesh::FPropHandleT<Polygon*> _fp_polygon_handle;
};


#endif
