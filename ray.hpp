#ifndef RAY
#define RAY

#include "vec3.hpp"

class ray {
    public:
    ray() {}
    ray(const vec3& a, const vec3& b, float time = 0.0f) {A=a;B=b; _time = time;}
    vec3 origin() const {return A;}
    vec3 direction() const {return B;}
    vec3 pointAtParameter(float t) const {return A +t*B;}
    float time() const {return _time;}
    vec3 A, B;
    float _time;
};
#endif