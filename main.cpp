#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <vector>
#include <future> 
#include <mutex>
#include <chrono>
#include <algorithm>
#include <ctime>

#include "sphere.hpp"
#include "moving_sphere.hpp"
#include "box.hpp"
#include "hitableList.hpp"
#include "float.h"
#include "camera.hpp" 
#include "material.hpp"
#include "bvh_node.hpp"

#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int rayCount = 0;

vec3 color(const ray& r, hitable *world, int depth) {
    rayCount++;
    hit_record rec;
    if (world->hit(r, 0.001f, 1000.0f, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * color(scattered, world, depth+1);
        } else {
            return emitted;
        }
    } else { // Sky color
        vec3 unitDir = unit_vector(r.direction());
        float t = 0.5f * (unitDir.y() +1.0f);
        return (1.0f-t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
        // return vec3(0.0f);
    }   
}

hitable *random_scene() {
    int sizeScene = 11;
    int n = 500;
    hitable **list = new hitable*[n+1];
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9)));
    list[0] = new sphere(vec3(0, -1000.0f, 0.0f), 1000.0f, new lambertian(checker));
    int i = 1;
    for (int a = -sizeScene; a < sizeScene; a++) {
        for (int b=-sizeScene; b < sizeScene; b++) {
            float choose_mat = frandom();
            vec3 center = vec3(a+0.9*frandom(), 0.2f, b+0.9f*frandom());
            if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
                if (choose_mat < 0.25) {
                    list[i++] = new sphere(center, 0.2f, 
                    new lambertian(new constant_texture(vec3(frandom()*frandom(), frandom()*frandom(), frandom()*frandom()))));
                } else if (choose_mat < 0.5) {
                    list[i++] = new sphere(center, 0.2f,
                                new metal(vec3(0.5f*(1.0f+ frandom()),0.5*(1.0+ frandom()),0.5*(1.0+ frandom())), 0.5* frandom()));
                } else if (choose_mat < 0.75) {
                    list[i++] = new sphere(center, 0.2f, new diffuse_light(new constant_texture(vec3(4.0f))));
                } else {
                    list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0f, new lambertian(new constant_texture(vec3(0.4f, 0.2f, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
    return new hitableList(list, i);
}

hitable *two_spheres() {
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9)));
    hitable **list = new hitable*[2];
    list[0] = new sphere(vec3(0, -10,0), 10, new lambertian(checker));
    list[1] = new sphere(vec3(0, 10,0), 10, new lambertian(checker));
    
    return new hitableList(list, 2);
}

hitable *two_perlin_spheres() {
    int nx, ny, nn;
    unsigned char* tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
    std::cout << "w: " << nx << " h: " << ny << " c: " << nn << std::endl;
    std::cout << "random pixel: " << int(tex_data[100]) << std::endl;
    std::cout << "random pixel: " << int(tex_data[200]) << std::endl;
    texture *pertext = new noise_texture(4.0f);
    hitable **list = new hitable*[2];

    list[0] = new sphere(vec3(0, -1000,0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(new image_texture(tex_data, nx, ny)));
    
    return new hitableList(list, 2);
}

hitable *simple_light() {
    texture *pertext = new noise_texture(4.0f);    
    hitable **list = new hitable*[4];

    list[0] = new sphere(vec3(0, -1000,0), 1000, new lambertian(pertext));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4.0f))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4.0f))));
    
    return new hitableList(list, 4);
}

hitable *scene_new() {
    texture *pertext = new noise_texture(4.0f);    
    hitable **list = new hitable*[15];
    int i = 0;
    list[i++] = new sphere(vec3(0, -1000,0), 1000,new checker_material(new constant_texture(vec3(0.04f, 0.71f, 0.7f)), vec3(0.7f, 0.6f, 0.5f), 0.0f));
    list[i++] = new sphere(vec3(0, 1, 1), 1, new lambertian(pertext));
    list[i++] = new sphere(vec3(0, 1, -1), 1.0f, new dielectric(1.5f));
    list[i++] = new sphere(vec3(0, 1, 3), 1.0f, new lambertian(new constant_texture(vec3(0.4f, 0.2f, 0.1))));
    list[i++] = new sphere(vec3(0, 1, -3), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
    list[i++] = new sphere(vec3(0, 25.0f, -10), 20.0f, new diffuse_light(new constant_texture(vec3(1.0f))));
    
    return new hitableList(list, i);
}

hitable *scene_fuzz() {
    hitable **list = new hitable*[15];
    int i = 0;
    list[i++] = new sphere(vec3(0, -1000,0), 1000,new checker_material(new constant_texture(vec3(0.04f, 0.71f, 0.7f)), vec3(0.7f, 0.6f, 0.5f), 0.0f));
    list[i++] = new sphere(vec3(0, 1, -6), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
    list[i++] = new sphere(vec3(0, 1, -4), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.15f));
    list[i++] = new sphere(vec3(0, 1, -2), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.30f));
    list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.45f));
    list[i++] = new sphere(vec3(0, 1, 2), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.70f));
    list[i++] = new sphere(vec3(0, 1, 4), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.85f));
    list[i++] = new sphere(vec3(0, 1, 6), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 1.0f));
    
    return new hitableList(list, i);
}

hitable *scene_map() {
    texture *pertext = new noise_texture(4.0f);    
    hitable **list = new hitable*[1];
    int i = 0;
    list[i++] = new sphere_map(vec3(0), 1000, new lambertian(new constant_texture(vec3(0.4f, 0.2f, 0.1))));
    list[i++] = new sphere(vec3(0), 1, new lambertian(pertext));

    return new hitableList(list, i);
}

hitable *cornell_box() {
    hitable **list = new hitable*[12];
    int i = 0;
    material * red = new lambertian(new constant_texture(vec3(0.65f, 0.05f, 0.05f)));
    material * white = new lambertian(new constant_texture(vec3(0.73f)));
    material * green= new lambertian(new constant_texture(vec3(0.12f, 0.45f, 0.15f)));    
    material * light = new diffuse_light(new constant_texture(vec3(15.0f)));

    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, new checker_material(new constant_texture(vec3(11.0f/255.0f, 183.0f/255.0f,180.0f/255.0f)), vec3(0.7f, 0.6f, 0.5f), 0.0f));//white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new sphere(vec3(350, 301, 377), 70.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
    list[i++] = new sphere(vec3(300, 81, 170), 80.f, new dielectric(1.5f));        
    list[i++] = new sphere(vec3(150, 81, 377), 80.0f, new lambertian(new constant_texture(vec3(0.0f, 0.5f, 0.5))));

    //list[i++] = new translate(new rotate_y(new box(vec3(0.0f), vec3(165), white), -18), vec3(130, 0, 65));
    list[i++] = new translate(new rotate_y(new box(vec3(0.0f), vec3(165, 200, 165), white), 15), vec3(265, 0, 295));
   
    return new hitableList(list, i);
}

hitable *final_image() {
    int nb = 20;
    hitable **list = new hitable*[30];
    hitable **boxlist = new hitable*[10000];
    hitable **boxlist2 = new hitable*[10000];
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *ground = new lambertian( new constant_texture(vec3(0.48, 0.83, 0.53)) );
    
    int b = 0;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float w = 100;
            float x0 = -1000 + i*w;
            float z0 = -1000 + j*w;
            float y0 = 0;
            float x1 = x0 + w;
            float y1 = 100*(glm::linearRand(0.0f, 1.0f)+0.01);
            float z1 = z0 + w;
            boxlist[b++] = new box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground);
        }
    }
    int l = 0;
    list[l++] = new bvh_node(boxlist, b, 0, 1);
    material *light = new diffuse_light( new constant_texture(vec3(7, 7, 7)) );
    list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
    vec3 center(400, 400, 200);
    list[l++] = new moving_sphere(center, center+vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
    list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
    list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
    hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    list[l++] = boundary;
    //list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
    //boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
    //list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
    material *emat =  new lambertian(new image_texture(tex_data, nx, ny));
    list[l++] = new sphere(vec3(400,200, 400), 100, emat);
    texture *pertext = new noise_texture(0.1);
    list[l++] =  new sphere(vec3(220,280, 300), 80, new lambertian( pertext ));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxlist2[j] = new sphere(vec3(165*frandom(), 165*frandom(), 165*frandom()), 10, white);
    }
    list[l++] =   new translate(new rotate_y(new bvh_node(boxlist2,ns, 0.0, 1.0), 15), vec3(-100,270,395));
    return new hitableList(list,l);
}

struct rayColor {
    int id;
    vec3 color;
    rayColor(int i,int r, int g, int b): id(i), color(r, g, b) {}
};

const int nx = 1200;
const int ny = 800;
const int ns = 100;

//#define SINGLE_THREAD

std::mutex mutex;
int main() {
    std::cout << "cores: "<< std::thread::hardware_concurrency() << std::endl;
    //vec3 lookFrom(278, 278, -800);
    //vec3 lookAt(278, 278,0);

    vec3 lookFrom(15, 3 ,2);
    vec3 lookAt(0.0, 0.0, 0.0);

    float dist_to_focus = 10.0f;
    float aperture = 0.0f;
    camera cam(lookFrom, lookAt, vec3(0,1,0), 40, float(nx)/float(ny), aperture, dist_to_focus, 0.0f, 1.0f);

    //hitable *world = random_scene();
    //hitable *world = two_perlin_spheres();
    //hitable *world = simple_light();
   // hitable *world = cornell_box();
    //hitable *world = final_image();
    hitable *world = scene_fuzz();
    //hitable *world = scene_map();
   
    int pixelCount;
    std::vector<rayColor> *pixelsColor = new std::vector<rayColor>();
    pixelsColor->reserve(nx * ny);

    std::ofstream file("fuzz_metal.ppm", std::ofstream::trunc);
    if (!file.is_open()) {
        std::cout << "yep! file is not open" << std::endl;
    }
    file << "P3\n" << nx << " " << ny << "\n255\n";  

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
#ifdef SINGLE_THREAD
   
    for (int j=ny-1; j >= 0; j--) {
        for (int i=0; i < nx; i++) {
            vec3 col(0.0f);
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
#else // multi threading
    std::size_t cores = std::thread::hardware_concurrency();
    std::vector<std::future<void>> future_vector;
    int pixelsY = ny / cores;
    std::cout << "pixels for thread: " << pixelsY << " pixelsLeft: " << ny - (pixelsY*cores) << std::endl;
    for (std::size_t c(0); c < cores; c++) {
        future_vector.emplace_back(std::async(std::launch::async, [=, &world, &cam, &pixelsColor, &file, &pixelCount]() {
            int pixelID = nx * pixelsY * c;
            std::cout << "launch pixels : " << c <<  " init id: " << pixelID << std::endl;

            //for (int j=(pixelsY * c + pixelsY)-1; j >= pixelsY * c; j--) {
            for (int j = pixelsY * c; j < (pixelsY * c + pixelsY)-1; j++) {
                for (int i=nx-1; i >= 0; i--) {    
                    vec3 col(0.0f);
                    for (int n = 0; n < ns; n++) {
                        float u = float(i + frandom()) / float(nx);
                        float v = float(j + frandom()) / float(ny);
                        ray r = cam.get_ray(u, v);
                        col += color(r, world, 0);    
                    }
                    col /= float(ns);
                    col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
                    // int ir = int(255.99 * glm::pow(col[0], 0.4545f));
                    // int ig = int(255.99 * glm::pow(col[1], 0.4545f));
                    // int ib = int(255.99 * glm::pow(col[2], 0.4545f));
                    int ir = int(255.99 * col[0]);        
                    int ig = int(255.99 * col[1]);        
                    int ib = int(255.99 * col[2]);
                    // gamma color
                    mutex.lock();
                    pixelsColor->push_back(rayColor(pixelID++, ir, ig, ib));
                    mutex.unlock();
                }   
                if (pixelID % 1000 == 0) {
                    std::cout << c << ": pixels calculated " << pixelID << std::endl;
                }
            } 
            std::cout << c << " terminated" << std::endl;
        }));
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));

    for (int i = 0; i < future_vector.size(); i++) {
        future_vector.at(i).get();
    }
    
#endif 
    end = std::chrono::system_clock::now();
    std::cout << "render time: " << std::chrono::duration_cast<std::chrono::seconds>(end-start).count() << std::endl;

#ifndef SINGLE_THREAD
    std::sort(pixelsColor->begin(), pixelsColor->end(), [](rayColor a, rayColor b) {return a.id > b.id; });
    for (int i=0; i<pixelsColor->size(); i ++) {
        file << pixelsColor->at(i).color << "\n";                    
    }
#endif
    file.close();

    return 1;
}