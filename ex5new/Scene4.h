#ifndef cg_projects_Scene4_h
#define cg_projects_Scene4_h

struct Scene4 : public Scene
{
	
	Scene4()
	: Scene()
	{
		defineGeometry();
		defineLights();
	}
	
	void defineLights()
	{
		Scene & scene = *this;
		Point3d pos(-30,20,20);
		Color3d color(1,1,1);
		PointLight  * p = new PointLight(pos,color);
		scene.add_light(p);
		
		Point3d pos1(30,20,20);
		Color3d color1(1,1,1);
		PointLight  * p1 = new PointLight(pos1,color1);
		scene.add_light(p1);
	}
	
	void defineGeometry()
	{
		Scene& scene = *this;
#if !WITHOUT_TEXTURES
		BImage * b = new BImage("textures/checkerboard_lg.bmp");
		BImage * w = new BImage("textures/warning.bmp");
#endif
		/* define some colors */
		Color3d white(1.0);
		Color3d black(0.0);
		Color3d red(1.0,0.0,0.0);
		Color3d green(0.0,1.0,0.0);
		Color3d blue(0.0,0.0,1.0);
		Color3d iron( 0.30, 0.30, 0.30);
		
		scene.backgroundColor() = (blue +  white) * 0.5;
		
		// First sphere and string  //
		Point3d center0(-12,2,-4);
		double radius = 3;
		Sphere* sp0 = new Sphere(center0,radius);
		sp0->diffuse() = iron;
		sp0->reflection() = white*0.3;
		sp0->specular() = white;
		sp0->shining() = 60;
		scene.add_object(sp0);
		
	}
	
	virtual void setDefaultCamera(Camera& camera) const
	{
		Point3d pos(0,8,35);
		double fov_h = 30 / 180.0 * M_PI;
		Point3d coi(0,0,-0);
		Vector3d up(0,1,0) ;
		camera = Camera(pos,coi,up,fov_h);
	}
	virtual ~Scene4() {

	}

};


#endif
