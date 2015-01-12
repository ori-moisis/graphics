#ifndef cg_projects_Scene5_h
#define cg_projects_Scene5_h

struct Scene5 : public Scene
{
	
	Scene5()
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
		//scene.add_light(p);
		
		Point3d pos1(30,20,20);
		Color3d color1(1,1,1);
		PointLight  * p1 = new PointLight(pos1,color1);
		//scene.add_light(p1);

		Point3d pos2(0,0,1);
        Color3d color2(1,1,1);
        PointLight* p2 = new PointLight(pos2,color2);
        scene.add_light(p2);
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
		//scene.add_object(sp0);
		
		// Second sphere and string //
        Point3d center1(-6,2,-4);
        radius = 3;
        Sphere* sp1 = new Sphere(center1,radius);
        sp1->diffuse() = iron;
        sp1->reflection() = white*0.3;
        sp1->specular() = white;
        sp1->shining() = 60;
        //scene.add_object(sp1);

        // Third sphere and string  //
        Point3d center2(0,2,-4);
        radius = 3;
        Sphere* sp2 = new Sphere(center2,radius);
        sp2->diffuse() = iron;
        sp2->reflection() = white*0.3;
        sp2->specular() = white;
        sp2->shining() = 60;
#if !WITHOUT_TEXTURES
        sp2->set_texture_map(w);
#endif
        //scene.add_object(sp2);

        // Fourth sphere and string //
        Point3d center3(6,2,-4);
        radius = 3;
        Sphere* sp3 = new Sphere(center3,radius);
        sp3->diffuse() = iron;
        sp3->reflection() = white*0.3;
        sp3->specular() = white;
        sp3->shining() = 60;
        //scene.add_object(sp3);

        // Fifth sphere and string  //
        Point3d center4(17.5,4.0,-4.0);
        radius = 3;
        Sphere* sp4 = new Sphere(center4,radius);
        sp4->diffuse() = iron;
        sp4->reflection() = white*0.3;
        sp4->specular() = white;
        sp4->shining() = 60;
        //scene.add_object(sp4);


        {
            Point3d center(0,0,-1);
            double radius = 0.5;
            Sphere* sp = new Sphere(center,radius);
            sp->diffuse() = iron;
            sp->reflection() = white*0.3;
            sp->specular() = white;
            sp->shining() = 60;
            //scene.add_object(sp);
        }

//        {
//            Point3d center(0.3,0.3,0);
//            double radius = 0.1;
//            Sphere* sp = new Sphere(center,radius);
//            //sp->diffuse() = blue;
//            sp->reflection() = white * 0.5;
//            sp->transparency() = white * 0.5;
//            sp->index() = 2.0;
//            sp->specular() = white;
//            sp->shining() = 16;
//            scene.add_object(sp);
//        }

        {
            //create a plane
            vector<Point3d> plane(4);
            plane[0] = Point3d(-1.5,-1.5,-3);
            plane[1] = Point3d(-1.5,0.5,-2);
            plane[2] = Point3d(0.5,0.5,-2);
            plane[3] = Point3d(0.5,-1.5,-3);
            Polygon* poly = new Polygon(plane);
            poly->diffuse() = ((blue + red) * 0.5 + white * 0.5) * 0.2;
            poly->reflection() = (blue + red) * 0.5 + white * 0.5;
            //scene.add_object(poly);
        }

        {
            //create a plane
            vector<Point3d> plane(4);
            plane[0] = Point3d(-1,-1,-40);
            plane[1] = Point3d(1,-1,-40);
            plane[2] = Point3d(1,-1,2);
            plane[3] = Point3d(-1,-1,2);
            Polygon* poly = new Polygon(plane);
            //poly->diffuse() = ((blue + red) * 0.5 + white * 0.5) * 0.2;
            poly->diffuse() = blue;
            //poly->reflection() = (blue + red) * 0.5 + white * 0.5;
            scene.add_object(poly);
        }



	}
	
	virtual void setDefaultCamera(Camera& camera) const
	{
	    Point3d pos(0,0,1);
		double fov_h = 30 / 180.0 * M_PI;
		Point3d coi(0,0,-1);
		Vector3d up(0,1,0) ;
		camera = Camera(pos,coi,up,fov_h);
	}
	virtual ~Scene5() {

	}

};


#endif