#include "scene.h"

#include "sphere.h"

Scene::Scene()
: _background (COLOR_BLACK)
, _numberOfRefRays (1)
, _numberOfShadowRays(NUM_SHADOW_RAYS_FACTOR)
, _cutoffAngle (0) {
}

Scene::Scene(Color3d& color, AmbientLight& light, double cutoffAngle)
: _ambientLight (light)
, _background (color)
, _numberOfRefRays(1)
, _numberOfShadowRays(NUM_SHADOW_RAYS_FACTOR)
, _cutoffAngle(cutoffAngle) {
}


Vector3d doReflect(const Vector3d& I, const Vector3d& N) {
    return I - (2 * OpenMesh::dot(N, I) * N);
}

Color3d doPhong(const Object& obj,
                const Color3d& texColor,
                const Point3d& position,
                const Vector3d& normal,
                const Vector3d& toEye,
                const Point3d& lightPos,
                const Color3d& lightColor) {

    Vector3d toLight = lightPos - position;
    toLight.normalize();

    Vector3d reflect = doReflect(-toLight, normal);
    reflect.normalize();

    Color3d diffuseColor = obj.getDiffuse() * lightColor * std::max(0.0, OpenMesh::dot(toLight, normal));
    Color3d specularColor = obj.getSpecular() * lightColor * pow(std::max(0.0, OpenMesh::dot(reflect, toEye)), obj.getShining());

    Color3d outColor = diffuseColor * texColor + specularColor;
    return outColor;
}

Color3d Scene::trace_ray(Ray ray, double vis, bool inObject) const {
    Object* obj;
    double t;
    Point3d intersectionPoint;
    Vector3d intersectionNormal;
    Color3d texColor(COLOR_WHITE);
    Color3d outColor(this->_background);
    if (vis < MINIMAL_VIS) {
        return outColor;
    }

    if (this->findNearestObject(ray, &obj, t, intersectionPoint, intersectionNormal, texColor)) {
        outColor = COLOR_BLACK;

        // Fix normal direction to be opposite the incoming ray
        if (OpenMesh::dot(intersectionNormal, ray.D()) > 0) {
            intersectionNormal = -intersectionNormal;
        }

        // New visability is the max component of reflect / refract (maximum 0.5 so we don't recurse forever)
        double visFactor = std::max(std::max(obj->getTransparency()[0], obj->getTransparency()[1]), obj->getTransparency()[2]);
        visFactor = std::max(std::max(std::max(visFactor, obj->getReflection()[0]), obj->getReflection()[1]), obj->getReflection()[2]);
        vis = vis * std::min(RECURSION_FACTOR, visFactor);

        // Reflect
        Color3d lastReflectColor = COLOR_BLACK;
        bool haveReflect = obj->getReflection() != COLOR_BLACK;
        if (haveReflect) {
            this->calcReflection(ray, intersectionPoint, intersectionNormal, vis, inObject, lastReflectColor);
            outColor += obj->getReflection() * lastReflectColor;
        }

        // Refract
        if (obj->getTransparency() != COLOR_BLACK) {
            outColor += obj->getTransparency() *
                    this->calcRefraction(ray, intersectionPoint, intersectionNormal, *obj, vis, inObject, haveReflect, lastReflectColor);
        }

        for (vector<PointLight*>::const_iterator light_iter = this->_lights.begin();
             light_iter != this->_lights.end ();
             ++light_iter) {

            // Check shadows
            double shadowRatio = this->calcShadowRatio(intersectionPoint, (*light_iter)->_position, NULL);

            if (shadowRatio == 0) {
                outColor += doPhong(*obj,
                                    texColor,
                                    intersectionPoint,
                                    intersectionNormal,
                                    -ray.D(),
                                    (*light_iter)->_position,
                                    (*light_iter)->_color);
            }
        }

        for (vector<SphereLight*>::const_iterator light_iter = this->_sphereLights.begin();
             light_iter != this->_sphereLights.end();
             ++light_iter) {

            // Check shadows
            double shadowRatio = this->calcShadowRatio(intersectionPoint, (*light_iter)->_C, *light_iter);
            Color3d color = doPhong(*obj,
                                    texColor,
                                    intersectionPoint,
                                    intersectionNormal,
                                    -ray.D(),
                                    (*light_iter)->_C,
                                    (*light_iter)->getSpecular());
            outColor += color * (1-shadowRatio);
        }

        // Add ambient
        outColor += this->_ambientLight._color;

        // Clamp values to [0..1]
        for (int i = 0; i < 3; ++i) {
            outColor[i] = std::max(0.0, std::min(1.0, outColor[i]));
        }
    }
    return outColor;
}

void Scene::add_object(Object* obj) {
    this->_objects.push_back(obj);
}

void Scene::add_light(PointLight* light) {
    this->_lights.push_back(light);
}

void Scene::add_sphere_light(SphereLight* light) {
    this->_sphereLights.push_back(light);
}

double randAngle(double maxDegrees) {
    return (((double) rand() / RAND_MAX) * maxDegrees) * M_PI / 180;
}

Vector3d randVector(double length) {
    Vector3d vec((double) rand() / RAND_MAX, (double) rand() / RAND_MAX, (double) rand() / RAND_MAX);
    vec.normalize();
    return vec * length;
}

Ray Scene::perturbateRay(const Ray& r) const {
    double randLen = this->_cutoffAngle / 45.0;
    Vector3d D = r.D() + randVector(randLen);
    return Ray(r.O(), D);
}

bool Scene::findNearestObject(Ray ray, Object** object, double& t, Point3d& P,
        Vector3d& N, Color3d& texColor, Object* additionalObject) const {
    double currT = MAX_DEPTH;
    t = MAX_DEPTH;
    Point3d currP;
    Vector3d currN;
    Color3d currColor;
    bool found = false;

    if (additionalObject) {
        if (additionalObject->intersect(ray, t, currT, currP, currN, currColor)) {
            t = currT;
            P = currP;
            N = currN;
            texColor = currColor;
            *object = additionalObject;
            found = true;
        }
    }

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
        const Vector3d& N, double vis, bool inObject, Color3d& lastReflectColor) const {

    Ray reflectedRay(P, doReflect(ray.D(), N));
    Color3d reflectColor = COLOR_BLACK;

    if (this->_numberOfRefRays == 1) {
        lastReflectColor = this->trace_ray(reflectedRay, vis, inObject);
    }
    else {
        for (int i = 0; i < this->_numberOfRefRays; ++i) {
            Ray randomRay = this->perturbateRay(reflectedRay);
            reflectColor += this->trace_ray(randomRay, vis, inObject);
        }
        lastReflectColor = reflectColor / this->_numberOfRefRays;
    }
}

Color3d Scene::calcRefraction(const Ray& ray, const Point3d& P,
        const Vector3d& N, const Object& object, double vis, bool inObject, bool haveReflect, Color3d& lastReflectColor) const {

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
            this->calcReflection(ray, P, N, vis, inObject, lastReflectColor);
        }
        return lastReflectColor;
    }
}

double Scene::calcShadowRatio(const Point3d& P, const Point3d& lightCenter, SphereLight* lightObj) const {
    double shadowRatio = 0;

    Object* shadowObj;
    Point3d shadowP;
    Vector3d shadowN;
    Color3d shadowColor;
    double shadowT;
    if (lightObj) {
        for (int i = 0; i < this->_numberOfShadowRays; ++i) {
            // Randomize a point on the light source
            Ray toLight(P, lightCenter + randVector(lightObj->_r) - P);
            if (this->findNearestObject(toLight, &shadowObj, shadowT, shadowP, shadowN, shadowColor, lightObj)) {
                if (shadowObj != lightObj) {
                    shadowRatio += 1 / this->_numberOfShadowRays;
                }
            }
        }
    } else {
        Vector3d vecToLight = lightCenter - P;
        Ray toLight(P, vecToLight);
        if (this->findNearestObject(toLight, &shadowObj, shadowT, shadowP, shadowN, shadowColor)) {
            if (shadowT < vecToLight.length()) {
                shadowRatio = 1;
            }
        }
    }

    return shadowRatio;
}
