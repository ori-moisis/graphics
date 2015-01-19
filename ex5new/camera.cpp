#include "camera.h"

#include "bimage.h"
#include "glm/ext.hpp"
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
, _transform (glm::inverse(glm::lookAt (glm::vec3(pos[0], pos[1], pos[2]),
                                        glm::vec3(coi[0], coi[1], coi[2]),
                                        glm::vec3(up[0], up[1], up[2])))) {

}

void Camera::render(size_t row_start, size_t number_of_rows, BImage& img,
                    Scene& scene) const {

    srand(0);

    double fov_v = this->_fov_h * img.width() / img.height();

    for (int i = row_start; i < row_start + number_of_rows; ++i) {
        if (DO_PRINTS) std::cout << "starting row " << i << std::endl;
        for (int j = 0; j < img.width(); ++j) {
            Color3d color = COLOR_BLACK;

            for (int k = 0; k < this->_samples_per_pixel; ++k) {
                double rowOffset = 0.5;
                double colOffset = 0.5;

                if (this->_samples_per_pixel > 1) {
                    rowOffset = (double)rand() / RAND_MAX;
                    colOffset = (double)rand() / RAND_MAX;
                }

                double rowDir = (img.height() / 2 - rowOffset - i) * tan(fov_v) * 2 / img.height();
                double colDir = (colOffset + j - img.width() / 2) * tan(this->_fov_h) * 2 / img.width();

                glm::vec4 dir(colDir, rowDir, -1, 0);
                dir = this->_transform * dir;

                Ray r (this->_position, Vector3d(dir.x, dir.y, dir.z));
                if (DO_PRINTS) std::cout << "Tracing ray at pixel " << j << ',' << i << " sample #" << k << std::endl;

                Color3d rayColor = scene.trace_ray(r);

                color += rayColor;
            }

            color = color / this->_samples_per_pixel;

            img(j, i) = Bpixel(color[0] * 255, color[1] * 255, color[2]* 255);
        }
    }
}

void Camera::setSamplesPerPixel(size_t samples_per_pixel) {
    this->_samples_per_pixel = samples_per_pixel;
}
