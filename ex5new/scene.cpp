#include "scene.h"

#include "sphere.h"

Scene::Scene()
: _background (COLOR_BLACK)
, _cutoffAngle (0)
, _numberOfRefRays (1) {
}

Scene::Scene(Color3d& color, AmbientLight& light, double cutoffAngle)
: _ambientLight (light)
, _background (color)
, _cutoffAngle(cutoffAngle)
, _numberOfRefRays(1) {
}


Vector3d doReflect(const Vector3d& I, const Vector3d& N) {
    return I - (2 * OpenMesh::dot(N, I) * N);
}

Color3d doPhong(const Object& obj,
                const Point3d& position,
                const Vector3d& normal,
                const Vector3d& toEye,
                const PointLight& light) {

    Vector3d toLight = light._position - position;
    toLight.normalize();

    Vector3d reflect = doReflect(-toLight, normal);
    reflect.normalize();

    Color3d diffuseColor = obj.getDiffuse() * light._color * std::max(0.0, OpenMesh::dot(toLight, normal));
    Color3d specularColor = obj.getSpecular() * light._color * pow(std::max(0.0, OpenMesh::dot(reflect, toEye)), obj.getShining());

    Color3d outColor = diffuseColor + specularColor;
    return outColor;
}

Color3d Scene::trace_ray(Ray ray, double vis, bool inObject) const {
    Object* obj;
    double t;
    Point3d intersectionPoint;
    Vector3d intersectionNormal;
    Color3d texColor(this->_background);
    Color3d outColor(this->_background);
    if (vis < 0.005) {
        return outColor;
    }

    if (this->findNearestObject(ray, &obj, t, intersectionPoint, intersectionNormal, texColor)) {
        std::cout << "hit object " << *(int*)&obj << " at point " << intersectionPoint << std::endl;
        outColor = COLOR_BLACK;

        vis = vis/2;

        // Fix normal direction to be opposite the incoming ray
        if (OpenMesh::dot(intersectionNormal, ray.D()) > 0) {
            intersectionNormal = -intersectionNormal;
        }

        // Reflect
        bool haveReflect = obj->getReflection() != COLOR_BLACK;
        if (haveReflect) {
            this->calcReflection(ray, intersectionPoint, intersectionNormal, vis, inObject);
            outColor += obj->getReflection() * this->_lastReflection;
        }

        // Refract
        if (obj->getTransparency() != COLOR_BLACK) {
            outColor += obj->getTransparency() *
                    this->calcRefraction(ray, intersectionPoint, intersectionNormal, *obj, vis, inObject, haveReflect);
        }

        for (vector<PointLight*>::const_iterator light_iter = this->_lights.begin();
             light_iter != this->_lights.end ();
             ++light_iter) {

            // Check shadows
            bool hasShadow = false;
            Vector3d vecToLight = (*light_iter)->_position - intersectionPoint;
            Ray toLight(intersectionPoint, vecToLight);

            Object* shadowObj;
            Point3d shadowP;
            Vector3d shadowN;
            Color3d shadowColor;
            double shadowT;

            if (this->findNearestObject(toLight, &shadowObj, shadowT, shadowP, shadowN, shadowColor)) {
                if (shadowT < vecToLight.length()) {
                    hasShadow = true;
                }
            }

            if (! hasShadow) {
                outColor += doPhong(*obj,
                                    intersectionPoint,
                                    intersectionNormal,
                                    -ray.D(),
                                    **light_iter);
            }
        }
        // Clamp values to [0..1]
        for (int i = 0; i < 3; ++i) {
            outColor[i] = std::max(0.0, std::min(1.0, outColor[i]));
        }
    }
    return outColor;
}

void Scene::add_object(Object* obj) {
    std::cout << "Adding object" << std::endl;
    this->_objects.push_back(obj);
}

void Scene::add_light(PointLight* light) {
    this->_lights.push_back(light);
}

double randAngle(double maxDegrees) {
	return (((double) rand() / RAND_MAX) * maxDegrees) * M_PI / 180;
}

Ray Scene::perturbateRay(const Ray& r) const {
	double rayTheta = atan2(r.D()[1], r.D()[0]);
	double rayPhi = acos(r.D()[2]);	// r=1, so z/r == z
	double randTheta = rayTheta + randAngle(this->_cutoffAngle);
	double randPhi = rayPhi + randAngle(this->_cutoffAngle);
	double x = cos(randTheta) * sin(randPhi);
	double y = sin(randTheta) * sin(randPhi);
	double z = cos(randPhi);
	Vector3d D = Vector3d(x, y, z);
	Ray randomRay = Ray(r.O(), D);
	return randomRay;
}

bool Scene::findNearestObject(Ray ray, Object** object, double& t, Point3d& P,
        Vector3d& N, Color3d& texColor) const {
    double currT = MAX_DEPTH;
    t = MAX_DEPTH;
    Point3d currP;
    Vector3d currN;
    Color3d currColor;
    bool found = false;
    for (std::vector<Object*>::const_iterator obj_iter = this->_objects.begin();
         obj_iter != this->_objects.end();
         ++obj_iter) {

        if ((*obj_iter)->intersect(ray, t, currT, currP, currN, currColor)) {
            t = currT;
            P = currP;
            N = currN;
            texColor = currColor;
            *object = *obj_iter;
            found = true;
        }
    }
    return found;
}

void Scene::calcReflection(const Ray& ray, const Point3d& P,
        const Vector3d& N, double vis, bool inObject) const {

    Ray reflectedRay(P, doReflect(ray.D(), N));
	Color3d reflectColor = COLOR_BLACK;

	if (this->_numberOfRefRays == 1) {
		this->_lastReflection = this->trace_ray(reflectedRay, vis, inObject);
	}
	else {
		for (int i = 0; i < this->_numberOfRefRays; ++i) {
			Ray randomRay = this->perturbateRay(reflectedRay);
			//std::cout << "random ray #" << i << " direction: " << "theta=" << randTheta << " phi=" << randPhi << " [" << x << ", " << y << ", " << z <<"]" << std::endl;
			reflectColor += this->trace_ray(randomRay, vis, inObject);
		}
		this->_lastReflection = reflectColor / this->_numberOfRefRays;
	}
}

Color3d Scene::calcRefraction(const Ray& ray, const Point3d& P,
        const Vector3d& N, const Object& object, double vis, bool inObject, bool haveReflect) const {

    double index = inObject ? object.getIndex() : (1/object.getIndex());
    double c1 = -OpenMesh::dot(ray.D(), N);
    double c2 = 1-index*index*(1-(c1*c1));

    if (c2 > 0) {
        Vector3d refractedD = (index * ray.D()) + (index*c1 - sqrt(c2)) * N;
        Ray refractedRay(P, refractedD);
        if (this->_numberOfRefRays == 1) {
        	return this->trace_ray(refractedRay, vis, !inObject);
        }
        else {
        	Color3d refractColor = COLOR_BLACK;
    		for (int i = 0; i < this->_numberOfRefRays; ++i) {
    			Ray randomRay = this->perturbateRay(refractedRay);
    			refractColor += this->trace_ray(randomRay, vis, !inObject);
    		}
    		return refractColor / this->_numberOfRefRays;
        }
    } else {
        if (! haveReflect) {
            this->calcReflection(ray, P, N, vis, inObject);
        }
        return this->_lastReflection;
    }
}
