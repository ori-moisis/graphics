#include "scene.h"

#include "sphere.h"

Scene::Scene()
: _background (COLOR_BLACK)
, _lastReflection (COLOR_BLACK)
, _cutoffAngle (0)
, _numberOfRefRays (0) {
}

Scene::Scene(Color3d& color, AmbientLight& light, double cutoffAngle)
: _background (color)
, _ambientLight (light)
, _cutoffAngle(cutoffAngle)
, _numberOfRefRays(0) {
}

std::ostream& operator<<(std::ostream& stream,
                         const Color3d& c) {
    stream << '(' << c[0] << ',' << c[1] << ',' << c[2] << ')';
    return stream;
 }

Color3d doPhong(const Object& obj,
                const Point3d& position,
                const Point3d& normal,
                const Vector3d& toEye,
                const PointLight& light) {

    Vector3d toLight = light._position - position;
    toLight.normalize();

    Vector3d reflect = (2 * OpenMesh::dot(toLight, normal) * normal) - toLight;
    reflect.normalize();

    Color3d diffuseColor = obj.getDiffuse() * light._color * std::max(0.0, OpenMesh::dot(toLight, normal));
    Color3d specularColor = obj.getSpecular() * light._color * pow(std::max(0.0, OpenMesh::dot(reflect, toEye)), obj.getShining());

    Color3d outColor = diffuseColor + specularColor;
    return outColor;
}

Color3d Scene::trace_ray(Ray ray, double vis) const {
    Object* obj;
    double t;
    Point3d intersectionPoint;
    Point3d intersectionNormal;
    Color3d texColor(this->_background);
    Color3d outColor(this->_background);
    if (this->findNearestObject(ray, &obj, t, intersectionPoint, intersectionNormal, texColor)) {
        outColor = COLOR_BLACK;
        // See if we can hit any light source
        if (obj->getSpecular() != COLOR_BLACK) {
            // Do reflection
        }

        if (obj->getTransparency() != COLOR_BLACK) {
            // Do refraction
        }

        for (vector<PointLight*>::const_iterator light_iter = this->_lights.begin();
             light_iter != this->_lights.end ();
             ++light_iter) {
            // TODO: check shadows
            outColor += doPhong(*obj,
                                intersectionPoint,
                                intersectionNormal,
                                -ray.D(),
                                **light_iter);
        }
        // Clamp values to [0..1]
        for (int i = 0; i < 3; ++i) {
            outColor[i] = std::max(0.0, std::min(1.0, outColor[i]));
        }
    }
    return outColor * 255;
}

void Scene::add_object(Object* obj) {
    std::cout << "Adding object" << std::endl;
    this->_objects.push_back(obj);
}

void Scene::add_light(PointLight* light) {
    this->_lights.push_back(light);
}

Ray Scene::perturbateRay(const Ray& r) const {
    // TODO: change this when we want multiple rays
    return r;
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

Color3d Scene::calcReflection(const Ray& ray, const Point3d& P,
        const Vector3d& N, const Object& object, double vis,
        bool isCritical) const {

    return Color3d(0.5,0.5,0.5);
}

Color3d Scene::calcRefraction(const Ray& ray, const Point3d& P,
        const Vector3d& N, const Object& object, double vis) const {
    return Color3d(0.5,0.5,0.5);
}
