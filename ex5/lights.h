//
//  lights.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//  Purpose : Declerae and implement classes representing ambient and
//            point light sources.
//

#ifndef __LIGHTS__
#define __LIGHTS__


//////////////////////////////
// Project Includes         //
//////////////////////////////

#include "general.h"
#include "MyVecs.h"


//////////////////////////////
// Class Decleration        //
//////////////////////////////

class AmbientLight 
{
public:
  AmbientLight(Color3d  color = COLOR_BLACK):_color(color){}
public:
  Color3d _color;

};


class PointLight 
{
public:
  PointLight(Point3d position = POINT_ORIGIN,Color3d color = COLOR_BLACK):
    _position(position),_color(color){}

  Point3d _position;
  Color3d _color;
};


#endif
