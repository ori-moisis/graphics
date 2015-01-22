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
		Color3d white(1.0);

		Scene & scene = *this;
		Point3d pos1(40,20,20);
		Color3d color1 = white * 0.7;
		SphereLight* p1 = new SphereLight(pos1, 1, color1);
		scene.add_sphere_light(p1);


		Point3d pos2(20,10,20);
		Color3d color2 = white * 0.7;
		PointLight* p2 = new PointLight(pos2, color2);
		scene.add_light(p2);
		
		Point3d pos3(0,0,20);
		Color3d color3 = white * 0.6;
		PointLight* p3 = new PointLight(pos3, color3);
		scene.add_light(p3);

		scene.ambientLight()._color = COLOR_BLACK;
	}
	
	void defineGeometry()
	{
		double radius;
		double angle;
		Point3d center;
		Vector3d axis;
		glm::mat4 T;
		double height;

		Scene& scene = *this;

		/* define some colors */
		Color3d white(1.0);
		Color3d black(0.0);
		Color3d red(1.0,0.0,0.0);
		Color3d green(0.0,1.0,0.0);
		Color3d blue(0.0,0.0,1.0);
		Color3d iron( 0.30, 0.30, 0.30);
		Color3d brown(0.4, 0.33, 0.2);
		scene.backgroundColor() = white; //(blue +  white) * 0.2;
		
		// background plane (mountains)
		BImage * mounatains = new BImage("textures/mountains.bmp");
		vector<Point3d> plane(4);
		vector<Point2d> plane_uv(4);
		double x = 12;
		double y = 12;
		double z = -20;
		plane[0] = Point3d(-x,y,z);
		plane[1] = Point3d(x,y,z);
		plane[2] = Point3d(x,-y,z);
		plane[3] = Point3d(-x,-y,z);
		plane_uv[0] = Point2d(1,0);
		plane_uv[1] = Point2d(0,0);
		plane_uv[2] = Point2d(0,1);
		plane_uv[3] = Point2d(1,1);
		Polygon * poly = new Polygon(plane,plane_uv);
		poly->diffuse() = white;
		#if !WITHOUT_TEXTURES
		poly->set_texture_map(mounatains);
		#endif
		scene.add_object(poly);

		double mainZ = -6;
		double mainX = -1.0;
		double mainY = 0;

		// Santa's body
		center = Vector3d(mainX,mainY,mainZ);
		radius = 1.0;
		T = glm::mat4(1.0, 0.0, 0.0, 0.0,
					  0.0, 1.2, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					  0.0, 0.0, 0.0, 1.0);

		BImage* coat = new BImage("textures/coat.bmp");
		Ellipsoid * body = new Ellipsoid(center, radius, T);
		body->diffuse() = white; //red * 0.7;
		body->specular() = white;
		body->reflection() = white*0.2;
		body->shining() = 20;
		#if !WITHOUT_TEXTURES
		body->set_texture_map(coat);
		#endif
		scene.add_object(body);

		// Left leg
		center = Point3d(mainX-0.2,mainY-0.2,mainZ + 0.3);
		angle = 15;
		height = 1.3;
		axis = Vector3d(0.1f, 1.0f, 0.0f);
		Cone* leg1 = new Cone(center, angle, axis, height);
		leg1->diffuse() = white * 0.8;
		leg1->reflection() = white*0.2;
		leg1->specular() = white;
		leg1->shining() = 60;
		scene.add_object(leg1);

		// Right leg
		center = Point3d(mainX+0.2,mainY-0.2,mainZ + 0.3);
		angle = 15;
		height = 1.3;
		axis = Vector3d(-0.1f, 1.0f, 0.0f);
		Cone* leg2 = new Cone(center, angle, axis, height);
		leg2->diffuse() = white * 0.8;
		//leg2->reflection() = white*0.2;
		leg2->specular() = white;
		leg2->shining() = 60;
		scene.add_object(leg2);

		// Neck
		center = Point3d(mainX,mainY+1.7,mainZ + 0.3);
		angle = 20;
		height = 1.0;
		axis = Vector3d(0.0f, 1.0f, 0.0f);
		Cone* neck = new Cone(center, angle, axis, height);
		neck->diffuse() = Color3d(1.0, 0.9, 0.8);
		neck->specular() = white;
		neck->shining() = 60;
		scene.add_object(neck);

		// Head
		center = Point3d(mainX,mainY+1.6,mainZ + 0.3);
		radius = 0.5;
		Sphere * head = new Sphere(center, radius);
		head->diffuse() = Color3d(0.9, 0.8, 0.7);
		head->specular() = white * 0.2;
		head->shining() = 20;
		scene.add_object(head);

		// Left eye
		center = Point3d(mainX-0.22,mainY+1.7,mainZ + 0.75);
		radius = 0.05;
		Sphere * eye = new Sphere(center, radius);
		eye->diffuse() = brown;
		eye->shining() = 20;
		eye->transparency() = white * 0.2;
		scene.add_object(eye);

		// Right eye
		center = Point3d(mainX+0.22,mainY+1.7,mainZ + 0.75);
		radius = 0.05;
		eye = new Sphere(center, radius);
		eye->diffuse() = brown;
		eye->shining() = 20;
		eye->transparency() = white * 0.2;
		scene.add_object(eye);

		// Mouth
		center = Point3d(mainX+0.35,mainY+2.7,mainZ + 0.7);
		radius = 0.1;
		T = glm::mat4(1.5, 0.0, 0.0, 0.0,
					  0.0, 0.5, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					  0.0, 0.0, 0.0, 1.0);
		Ellipsoid* mouth = new Ellipsoid(center, radius, T);
		mouth->diffuse() = black;
		scene.add_object(mouth);

		// Hat
		center = Point3d(mainX,mainY+3.1,mainZ+0.3);
		angle = 20;
		height = 1.2;
		axis = Vector3d(-0.01f, 1.0f, -0.05f);
		Cone* hat = new Cone(center, angle, axis, height);
		hat->diffuse() = red * 0.7;
		hat->specular() = white * 0.2;
		hat->shining() = 10;
		scene.add_object(hat);

		// Pompom
		center = Point3d(mainX,mainY+3.1,mainZ+0.3);
		radius = 0.1;
		Sphere * pompom = new Sphere(center, radius);
		pompom->diffuse() = white;
		pompom->shining() = 20;
		scene.add_object(pompom);

		// Nose
		center = Point3d(mainX-0.12,mainY+1.8,mainZ + 2.0);
		radius = 0.1;
		T = glm::mat4(1.0, 0.0, 0.0, 0.0,
					  0.0, 1.0, 0.0, 0.0,
					  0.0, 0.0, 1.5, 0.0,
					  0.0, 0.0, 0.0, 1.0);
		Ellipsoid* nose = new Ellipsoid(center, radius, T);
		nose->diffuse() = red * 0.7;
		nose->shining() = 20;
		scene.add_object(nose);

		// Trees
		axis = Vector3d(0.0f, 1.0f, 0.0f);

		Cone* stump = new Cone(Point3d(mainX+2.5,1,mainZ-5), 15, axis, 3);
		stump->diffuse() = brown;
		scene.add_object(stump);
		Cone* top = new Cone(Point3d(mainX+2.5,3.5,mainZ-5), 20, axis, 3.2);
		top->diffuse() = green * 0.5;
		scene.add_object(top);

		stump = new Cone(Point3d(mainX+5,1.3,mainZ-7), 19, axis, 6);
		stump->diffuse() = brown*0.85;
		stump->reflection() = white*0.2;
		scene.add_object(stump);
		top = new Cone(Point3d(mainX+5,5.1,mainZ-7), 25, axis, 4.2);
		top->diffuse() = green * 0.4;
		top->reflection() = white*0.3;
		scene.add_object(top);

		stump = new Cone(Point3d(mainX+5,0.2,mainZ-3), 17, axis, 4);
		stump->diffuse() = brown*0.95;
		scene.add_object(stump);
		top = new Cone(Point3d(mainX+5,1.5,mainZ-3), 25, axis, 2);
		top->diffuse() = green * 0.3;
		top->reflection() = white*0.3;
		scene.add_object(top);

		// Lens
		center = Point3d(mainX+1.0,-0.3,-3);
		radius = 0.55;
		T = glm::mat4(0.8, 0.0, 0.0, 0.0,
					  0.0, 1.2, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					  0.0, 0.0, 0.0, 1.0);
		Ellipsoid * lens = new Ellipsoid(center,radius, T);
		lens->diffuse() = white * 0.1;
		lens->transparency() = white * 0.9;
		lens->specular() = white;
		lens->shining() = 64;
		lens->index() = 1.5;
		scene.add_object(lens);

		// Floor
		BImage * snow = new BImage("textures/snow.bmp");
		vector<Point3d> plane2(4);
		vector<Point2d> plane_uv2(4);
		double xClose = 20;
		double xFar = 5;
		double yClose = -1.5;
		double yFar = -1.5;
		z = 20;
		plane2[0] = Point3d(-xFar,yFar,-z);
		plane2[1] = Point3d(xFar,yFar,-z);
		plane2[2] = Point3d(xClose,yClose,0);
		plane2[3] = Point3d(-xClose,yClose,0);
		plane_uv2[0] = Point2d(1,0);
		plane_uv2[1] = Point2d(0,0);
		plane_uv2[2] = Point2d(0,1);
		plane_uv2[3] = Point2d(1,1);
		poly = new Polygon(plane2, plane_uv2);
		poly->diffuse() = white;
		poly->reflection() = white * 0.6;

		#if !WITHOUT_TEXTURES
		poly->set_texture_map(snow);
		#endif
		scene.add_object(poly);
	}

	virtual void setDefaultCamera(Camera& camera) const
	{
	    Point3d pos(0,0,0);
	    double fov_h = 30 / 180.0 * M_PI;
		Point3d coi(0,0,-5);
		Vector3d up(0,1,0) ;
		camera = Camera(pos,coi,up,fov_h);
	}

	virtual ~Scene4() {

	}

};


#endif
