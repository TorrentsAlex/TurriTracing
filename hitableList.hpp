#ifndef HITABLE_LIST
#define HITABLE_LIST

#include "hitable.hpp"

class hitableList : public hitable {
public:
    hitable **list;
    int list_size;
    hitableList() {}
    hitableList(hitable **l, int n)  { list = l; list_size = n;}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const ;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;

};

bool hitableList::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record tempRec;
    bool hitAnything = false;
    float closestSoFar = t_max;
    for (int i = 0; i < list_size;i++) {
        if (list[i]->hit(r, t_min, closestSoFar, tempRec)) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }
    return hitAnything;
}

bool hitableList::bounding_box(float t0, float t1, aabb& box) const {
    if (list_size < 1) return false;
    aabb temp_box;
    bool first_true = list[0]->bounding_box(t0, t1, temp_box);
    if (!first_true)
        return false;
    else 
        box = temp_box;
    for (int i = 1; i < list_size; i++) {
        if (list[i]->bounding_box(t0, t1, temp_box)) {
            box = surrounding_box(box, temp_box);
        } else {
            return false;
        }
    }
    return true;
}


#endif