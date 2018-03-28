#include <iostream>
#include <fstream>
#include <random>

#include "sphere.hpp"
#include "hitableList.hpp"
#include "float.h"
#include "camera.hpp"
#include "material.hpp"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, 100.0f, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth+1);
        } else {
            return vec3(0.0);
        }
    } else { // Sky color
        vec3 unitDir = unit_vector(r.direction());
        float t = 0.5f * (unitDir.y() +1.0f);
        return (1.0f-t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
    }   
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] = new sphere(vec3(0, -1000, 0.0f), 1000.0f, new lambertian(vec3(0.5f)));
    int i = 1;
    for (int a = -7; a < 7; a++) {
        for (int b=-7; b < 7; b++) {
            float choose_mat = frandom();
            vec3 center = vec3(a+0.9*frandom(), 0.2, b+0.9*frandom());
            if ((center - vec3(4,0.2, 0.0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(frandom()*frandom(), frandom()*frandom(), frandom()*frandom())));
                } else if (choose_mat < 0.95) {
                    list[i++] = new sphere(center, 0.2,
                                new metal(vec3(0.5*(1.0+ frandom()),0.5*(1.0+ frandom()),0.5*(1.0+ frandom())), 0.5* frandom()));
                } else {
                    list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0f, new lambertian(vec3(0.4f, 0.2f, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
    return new hitableList(list, i);
}

int main() {
    int nx = 200;
    int ny = 100;
    int ns = 100;
    vec3 lookFrom(13,3,2);
    vec3 lookAt(0,0,0);

    vec3 lightPos(20,20,20);
    vec3 lightDir = -lightPos;

    float dist_to_focus = 10.0f;
    float aperture = 0.1f;
    camera cam(lookFrom, lookAt, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    hitable *world = random_scene();
    
    // Open and create image
    std::ofstream file("image.ppm", std::ofstream::trunc);
    if (!file.is_open()) {
        std::cout << "yep! file is not open" << std::endl;
    }
    file << "P3\n" << nx << " " << ny << "\n255\n"; 
    int percentage = 0;
    
    for (int j=ny-1; j >= 0; j--) {
        for (int i=0; i < nx; i++) {
            vec3 col(0.0f);
            vec3 shad(0.f);
            //antialiasing code
            for (int n = 0; n < ns; n++) {
                float u = float(i + frandom()) / float(nx);
                float v = float(j + frandom()) / float(ny);
                ray r = cam.get_ray(u, v);
                
                col += color(r, world, 0);    
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);        
            int ig = int(255.99 * col[1]);        
            int ib = int(255.99 * col[2]);
            file << ir << " " << ig << " " << ib << "\n";   
        }
    }   
    file.close();
    return 0;
}