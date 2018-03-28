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

#endif