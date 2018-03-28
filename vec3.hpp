#ifndef VEC
#define VEC

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3 {
    public:
    float a[3];

    vec3() {}
    vec3(float x) {a[0] = x; a[1] = x; a[2] = x;}
    vec3(float x, float y, float j) {a[0] = x; a[1] = y; a[2] = j;}

    inline float x() const { return a[0];}
    inline float r() const { return a[0];}
    inline float y() const { return a[1];}
    inline float g() const { return a[1];}
    inline float z() const { return a[2];}
    inline float b() const { return a[2];}
    // Maths operators

    inline const vec3& operator+() const {return *this;}    
    inline vec3 operator-() const {return vec3(-a[0], -a[1], -a[2]);}
    inline float operator[](int i) const {return a[i];}
    inline float& operator[](int i) {return a[i];}
    
    // = Operator
    inline vec3& operator+=(const vec3 &v2) {
        a[0] += v2[0]; a[1] += v2[1]; a[2] += v2[2];
        return *this;
    } 
    inline vec3& operator-=(const vec3 &v2) {
        a[0] -= v2[0]; a[1] -= v2[1]; a[2] -= v2[2];
        return *this;
    }
    inline vec3& operator*=(const vec3 &v2) {
        a[0] *= v2[0]; a[1] *= v2[1]; a[2] *= v2[2];
        return *this;        
    }
    inline vec3& operator/=(const vec3 &v2) {
        a[0] /= v2[0]; a[1] /= v2[1]; a[2] /= v2[2];
        return *this;      
    }

    inline vec3& operator*=(const float &t) {
        a[0] *= t; a[1] *= t; a[2] *= t;
        return *this;        
    }
    inline vec3& operator/=(const float &t) {
        float k = 1.0f/t;
        a[0] *= k; a[1] *= k; a[2] *= k;
        return *this;        
    }

    inline float length() const {
        return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);}
    inline float squared_length() const {
        return a[0]*a[0]+a[1]*a[1]+a[2]*a[2];}
    inline void make_unit_vector();
};

inline std::istream& operator>>(std::istream &is, vec3 &t) {
    is >> t.a[0] >> t.a[1] >> t.a[2];
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
    os << t.a[0] << " " << t.a[1] << " " << t.a[2];
    return os;
}

inline void vec3::make_unit_vector() {
    float k = 1.0f / squared_length();
    a[0] *= k; a[1] *= k; a[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.a[0] + v2.a[0], v1.a[1] + v2.a[1], v1.a[2] + v2.a[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.a[0] - v2.a[0], v1.a[1] - v2.a[1], v1.a[2] - v2.a[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.a[0] * v2.a[0], v1.a[1] * v2.a[1], v1.a[2] * v2.a[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.a[0] / v2.a[0], v1.a[1] / v2.a[1], v1.a[2] / v2.a[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
    return vec3(t*v.a[0], t*v.a[1], t*v.a[2]);
}

inline vec3 operator*(const vec3 &v, float t) {
    return vec3(t*v.a[0], t*v.a[1], t*v.a[2]);
}

inline vec3 operator/(vec3 v, float t) {
    return vec3(v.a[0]/t, v.a[1]/t, v.a[2]/t);
}

/**
 * param: vec3
 * return vec3 / vec3.length
*/
inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline float dot(const vec3 &v1, const vec3 &v2) {
    return v1.a[0]*v2.a[0] + v1.a[1]*v2.a[1] + v1.a[2]*v2.a[2];
}

inline  vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3((v1.a[1]*v2.a[2]  - v1.a[2]*v2.a[1]),
                -(v1.a[0]*v2.a[2] - v1.a[2]*v2.a[0]),
                (v1.a[0]*v2.a[1] - v1.a[1]*v2.a[0]));
}

#endif