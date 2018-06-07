#ifndef MATERIAL
#define MATERIAL

struct hit_record;

#include "ray.hpp"
#include "hitable.hpp"
#include "texture.hpp"

/** return: float between [0.0, 1.0]*/
float frandom() {
    return glm::linearRand(0.0f, 1.0f);
}

inline vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0 * vec3(frandom(), frandom(), frandom()) - vec3(1.0);
    } while(p.squared_length() >= 1.0f);
    return p;
}

float schlick(float cosine, float ref_idx) {
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0*r0;
	return r0 + (1.0f-r0) * pow((1.0f - cosine), 5);
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v -2.0f*dot(v, n)*n;
}

bool refract(const vec3& v, const vec3& n, float n_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0f - n_over_nt * n_over_nt * (1.0f-dt*dt);
	if (discriminant > 0.0f) {
		refracted = n_over_nt * (uv - n * dt) -n*sqrt(discriminant);
		return true;
	} else {
		return false;
	}
}

class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(float u, float v, const vec3& p) const { return vec3(0.0f);}
};

class lambertian : public material {
public:
	texture* albedo;

	lambertian(texture* a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

};

class metal : public material {
public:
	vec3 albedo;
	float fuzz;

	metal(const vec3& a, float f) : albedo(a) {fuzz = f < 1 ? f: -1;}
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected- fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return dot(scattered.direction(), rec.normal) > 0;
	}
};

class PBR : public material {
public:
	texture* rough;
	texture* metal;
	texture* alb;
	float fuzz;
	PBR(texture* r, texture* m, texture* a): rough(r), metal(m), alb(a) {}
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal) * metal->value(rec.u, rec.v, rec.p).r();
		scattered = ray(rec.p, reflected- fuzz * random_in_unit_sphere());
		attenuation = alb->value(rec.u, rec.v, rec.p);
		return dot(scattered.direction(), rec.normal) > 0;
	}
};

class checker_material : public material {
	public:
	metal* met;
	lambertian* lam;
	checker_material(texture* diff, vec3 metallic, float fuzz) {
		met = new metal(metallic, fuzz); 
		lam = new lambertian(diff);
		//lam1 = new lambertian(new constant_texture(vec3(0.5f, 0.0f, 0.0f)));
	}
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const {
	   float sines = sin(4.0*rec.p.x())*sin(4.0*rec.p.y())*sin(4.0*rec.p.z());
        if (sines < 0) {
            return met->scatter(r_in, rec, attenuation, scattered);
        } else {
            return lam->scatter(r_in, rec, attenuation, scattered);
        }
	}
};

class dielectric : public material {
public:
	float ref_idx;
	dielectric(float fl): ref_idx(fl) {}
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const {
		vec3 out_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float n_over_nt;
		attenuation = vec3(1.0f, 1.0f, 1.0f);
		vec3 refracted;
		float reflected_prob;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0) {
			out_normal = -rec.normal;
			n_over_nt = ref_idx;
			cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
			//cosine = sqrt(1.0-ref_idx*ref_idx*(1.0*cosine*cosine));
		} else {
			out_normal = rec.normal;
			n_over_nt = 1.0f / ref_idx;
			cosine = - dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		
		if (refract(r_in.direction(), out_normal, n_over_nt, refracted)) {
			reflected_prob = schlick(cosine, ref_idx);
		} else {
			reflected_prob = 1.0f;
		}

		if (frandom() < reflected_prob) {
			scattered = ray(rec.p, reflected);
		} else {
			scattered = ray(rec.p, refracted);
		}
		
		return true;
	}
};

class diffuse_light : public material {
	public:
	texture *emit;
	diffuse_light() {}
	diffuse_light(texture *a) : emit(a) {}
	virtual bool scatter(const ray& r_in, const hit_record rec, vec3& attenuation, ray& scattered) const {
		return false;
	}

	virtual vec3 emitted(float u, float v, const vec3& p) const { return emit->value(u, v, p); }

};

#endif