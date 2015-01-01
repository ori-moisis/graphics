#include "camera.h"

#include "bimage.h"
#include "glm/gtc/matrix_transform.hpp"

#include "ray.h"
#include "scene.h"

Camera::Camera()
: _position (0.0, 0.0, 0.0)
, _coi (0.0, 0.0, -1.0)
, _up (0.0, 1.0, 0.0)
, _fov_h (30.0)
, _samples_per_pixel (1)
, _transform (1.0) {
}

Camera::Camera(Point3d& pos, Point3d& coi, Vector3d& up, double fov,
               size_t samples_per_pixel)
: _position (pos)
, _coi (coi)
, _up (up)
, _fov_h (fov)
, _samples_per_pixel (samples_per_pixel)
, _transform (glm::lookAt (glm::vec3(pos[0], pos[1], pos[2]),
                           glm::vec3(coi[0], coi[1], coi[2]),
                           glm::vec3(up[0], up[1], up[2]))) {

}

using namespace std;

void Camera::render(size_t row_start, size_t number_of_rows, BImage& img,
                    Scene& scene) const {
    // TODO: The transform might be inverse, or completely wrong...
    for (int i = row_start; i < row_start + number_of_rows; ++i) {
        double rowDir = ((0.5 + i) * tan(this->_fov_h) * 2) / img.height();
        for (int j = 0; j < img.width(); ++j) {
            double colDir = ((0.5 + j) * tan(this->_fov_h) * 2) / img.width();
            glm::vec4 dir(rowDir, colDir, 1, 0);
            dir = this->_transform * dir;

            Ray r (this->_position, Vector3d(dir.x, dir.y, dir.z));
            Color3d color = scene.trace_ray(r);
            img(i, j) = Bpixel(color[0], color[1], color[2]);
        }
    }
}

void Camera::setSamplesPerPixel(size_t samples_per_pixel) {
    this->_samples_per_pixel = samples_per_pixel;
}
