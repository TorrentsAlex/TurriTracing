#ifndef SPHERE
#define SPHERE

#include "hitable.hpp"

class sphere: public hitable {
    public:
    vec3 center;
    float radius;
    material  *mat_ptr;
    
    sphere() {}
    sphere(vec3 cen, float r, material *m): center(cen), radius(r), mat_ptr(m) {};
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;

};

bool sphere::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b -a*c;
    
    if (discriminant > 0) {
        float temp = (-b -sqrt(b*b-a*c))/a;
        if (temp < tmax && temp > tmin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);
            rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(b*b-a*c))/a;
        if (temp < tmax && temp > tmin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            get_sphere_uv((rec.p-center)/radius, rec.u, rec.v);            
            rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
        }
    }
    return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center - vec3(radius), center + vec3(radius));
    return true;
}

class sphere_map: public hitable {
    public:
    vec3 center;
    float radius;
    material  *mat_ptr;
    
    sphere_map() {}
    sphere_map(vec3 cen, float r, material *m): center(cen), radius(r), mat_ptr(m) {};
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;

};

bool sphere_map::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b -a*c;
    
    if (discriminant > 0) {
        float temp = (-b -sqrt(b*b-a*c))/a;
        if (temp < tmax && temp > tmin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            get_sphere_uv(-(rec.p-center)/radius, rec.u, rec.v);
            rec.normal = -(rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(b*b-a*c))/a;
        if (temp < tmax && temp > tmin) {
            rec.t = temp;
            rec.p = r.pointAtParameter(rec.t);
            get_sphere_uv(-(rec.p-center)/radius, rec.u, rec.v);            
            rec.normal = -(rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
        }
    }
    return false;
}

bool sphere_map::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center - vec3(radius), center + vec3(radius));
    return true;
}


#endif