//
//  SceneChooser.h
//  cg-projects
//
//  Created by HUJI Computer Graphics course staff, 2012-2013.
//

#ifndef cg_projects_SceneChooser_h
#define cg_projects_SceneChooser_h

#define WITHOUT_TEXTURES 0

#define MAX_SCENE_INDEX 3

#include "Scene1.h"
#include "Scene2.h"
#include "Scene3.h"
#include "Scene4.h"

/** Create a new scene according to the scene index and parameters. */
void setupScene(Scene** scene, size_t scene_index, size_t refRays, double cutoff)
{
	Scene* res;
	if (scene_index == 0)
	{
		res = new Scene1();
	}
	else if (scene_index == 1)
	{
		res = new Scene2();
	}
	else if (scene_index == 2)
	{
		res = new Scene3();
	}
	else if (scene_index == 3)
	{
	    res = new Scene4();
	}
	else
	{
		res = NULL;
		std::cout << "Specify scene index between 0 and 3.";
	}
	
	if (res)
	{
		res->setNumberOfRefRays(refRays);
		res->setCutoffAngle(cutoff);
	}
	
	*scene = res;
}

#endif
