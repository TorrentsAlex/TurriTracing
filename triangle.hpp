#ifndef TRIANGLE
#define TRIANGLE

#include "hitable.hpp"

float tsign(vec3 p1, vec3 v1, vec3 v2) {
	return (p1.x() - v2.x()) * (v1.y() - v2.y()) - 
        	(v1.x() - v2.x()) * (p1.y() - v2.y());
}

bool pointInTriangle(vec3 point, vec3 a, vec3 b, vec3 c) {
    bool b1,b2,b3;
    
    b1 = tsign(point, a, b) < 0.0;    
    b2 = tsign(point, b, c) < 0.0;
    b3 = tsign(point, c, a) < 0.0; 

	return ((b1 == b2) && (b2 == b3));
}

class triangle : public hitable {
    public:
    vec3 a, b, c;
    material *mat_ptr;

    triangle() {}
    triangle(vec3 p1, vec3 p2, vec3 p3, material *m) : a(p1), b(p2), c(p3), mat_ptr(m ) {}
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

};

bool triangle::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
    vec3 bc = b-c;
    vec3 ac = a-c;
    vec3 normal = cross(bc, ac);
    float D = dot(normal, a);
    //std::cout << "WARNING: triangle:hit don't return a hit_record, this cause a core dumped" << std::endl;
    if (dot(r.direction(), normal) > 0.0) {
        rec.t= D;
        rec.p = r.pointAtParameter(rec.t);
        rec.normal = normal;
        rec.mat_ptr = mat_ptr;
        return true;
    }
    return false;
}

#endif