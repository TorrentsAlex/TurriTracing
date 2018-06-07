#ifndef CAMERA
#define CAMERA

#include "ray.hpp"
float frandom();

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = 2.0*vec3(frandom(), frandom(), 0.0) - vec3(1.0, 1.0, 0.0);
    } while(dot(p, p) >= 1.0f);
    return p;
}

class camera {
public:
    vec3 origin; 
    vec3 lowerLeftCorner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float lens_radius;
    float _t0, _t1;
    camera(vec3 lookFrom, vec3 lookAt, vec3 vup, float vFov, float aspect, float aperture, float focus_dist, float time0, float time1) {
        _t0 = time0;
        _t1 = time1;
        lens_radius = aperture / 2.0f;
        float theta = vFov * 3.1416f /180;
        float half_height = tan(theta/2);
        float half_width = aspect * half_height;
        origin = lookFrom; 

        w = unit_vector(lookFrom - lookAt);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        lowerLeftCorner = origin - half_width*focus_dist*u -half_height*focus_dist*v - focus_dist*w;
        horizontal = 2.0f * half_width * focus_dist * u;
        vertical = 2.0f * half_height * focus_dist * v;
    }

    ray get_ray(float u, float v) {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        float time = _t0 + frandom() * (_t1 - _t0); 
        return ray(origin + offset, lowerLeftCorner + u*horizontal + v*vertical-origin-offset, time);
    }
};

#endif