#pragma once
#include "raytracer.h"
#include "math.h"

struct Plane
{
    Vec3 a;
    Vec3 n;
    Material mat;
};

struct Sphere
{
    Vec3 c;
    float r;
    Material mat;
};
