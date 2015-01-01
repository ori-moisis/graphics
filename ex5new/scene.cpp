#include "scene.h"

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

Color3d Scene::trace_ray(Ray ray, double vis) const {
    return Color3d(255,255,255);
}

void Scene::add_object(Object* obj) {
}

void Scene::add_light(PointLight* light) {
}

Ray Scene::perturbateRay(const Ray& r) const {
    return r;
}

bool Scene::findNearestObject(Ray ray, Object** object, double& t, Point3d& P,
        Vector3d& N, Color3d& texColor) const {
    return false;
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
