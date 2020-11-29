/* date = July 10th 2020 7:11 pm */

#ifndef WORLD_H
#define WORLD_H

#include "geometry.h"

struct World
{
    Plane *planes;
    Sphere *spheres;
    
    int maxPlanes;
    int maxSpheres;
    int planeCount;
    int sphereCount;
    
    Geometry *geometries;
    int geometryCount;
};

#endif //WORLD_H
