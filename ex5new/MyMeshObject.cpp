#include "MyMeshObject.h"

MyMeshObject::MyMeshObject(MyMesh& mesh)
: _mesh (mesh)
, _boundingSphere (NULL)
, _center (0,0,0) {
    this->calculateBoundingSphere();
}

MyMeshObject::~MyMeshObject() {
    if (this->_boundingSphere) {
        delete this->_boundingSphere;
        this->_boundingSphere = NULL;

        MyMesh::FaceIter end_iter = _mesh.faces_end();
        for (MyMesh::FaceIter iter = _mesh.faces_begin();
             iter != end_iter;
             ++iter)
        {
            delete _mesh.property(this->_fp_polygon_handle, iter);
            _mesh.property(this->_fp_polygon_handle, iter) = NULL;
        }
    }
}

void MyMeshObject::set_texture_map(BImage* image) {
    MyMesh::FaceIter end_iter = _mesh.faces_end();
    for (MyMesh::FaceIter iter = _mesh.faces_begin();
        iter != end_iter;
        ++iter)
    {
        Polygon* poly = _mesh.property(this->_fp_polygon_handle, iter);
        if (poly) {
            poly->set_texture_map(image);
        }
    }
}

int MyMeshObject::intersect(Ray& ray, double tMax, double& t, Point3d& P,
        Vector3d& N, Color3d& texColor) const {
    double tmpT;
    Point3d tmpP;
    Vector3d tmpN;
    Color3d tmpColor;

    bool found = false;
    if (this->_boundingSphere->intersect(ray, tMax, tmpT, tmpP, tmpN, tmpColor)) {
        tmpT = tMax;
        MyMesh::FaceIter end_iter = _mesh.faces_end();
        for (MyMesh::FaceIter iter = _mesh.faces_begin();
             iter != end_iter;
             ++iter) {
            Polygon* poly = _mesh.property(this->_fp_polygon_handle, iter);
            if (poly->intersect(ray, tmpT, tmpT, tmpP, tmpN, tmpColor)) {
                found = true;
            }
        }
    }

    if (found) {
        t = tmpT;
        P = tmpP;
        N = tmpN;
        texColor = tmpColor;
        return INTERSECTION;
    }

    return NO_INTERSECTION;
}

void MyMeshObject::calculateBoundingSphere() {
    for (MyMesh::VertexIter iter = _mesh.vertices_begin();
         iter != _mesh.vertices_end();
         ++iter) {
        Point3d p = _mesh.point(*iter);
        for (int j = 0; j < 3; ++j)
        {
            this->_center[j] += p[j];
        }
    }
    this->_center /= _mesh.n_vertices();

	double radius = 0;
	for (MyMesh::VertexIter iter = _mesh.vertices_begin();
		iter != _mesh.vertices_end();
		++iter) {
		double dist = (this->_center - _mesh.point(*iter)).length();
		radius = std::max(radius, dist);
	}

    this->_boundingSphere = new Sphere(this->_center, radius);

    // Create a polygon for each face
    _mesh.add_property(this->_fp_polygon_handle);

    bool textured = _mesh.has_vertex_texcoords2D();

    MyMesh::FaceIter end_iter = _mesh.faces_end();
    for (MyMesh::FaceIter iter = _mesh.faces_begin();
         iter != end_iter;
         ++iter)
    {
        MyMesh::Normal norm = _mesh.calc_face_normal(*iter);

        std::vector<Point3d> points;
        std::vector<Point2d> texPoints;

        MyMesh::FaceVertexIter end_iter = _mesh.fv_end(*iter);
        for (MyMesh::FaceVertexIter v_iter = _mesh.fv_begin(*iter); v_iter != end_iter; ++v_iter)
        {
            points.push_back(_mesh.point(v_iter.handle()));
            if (textured) {
                texPoints.push_back(_mesh.texcoord2D(v_iter.handle()));
            }
        }

        Polygon* poly = NULL;
        if (textured) {
            poly = new Polygon(points, texPoints, norm);
        } else {
            poly = new Polygon(points, norm);
        }
        _mesh.property(this->_fp_polygon_handle, iter) = poly;
    }
}
