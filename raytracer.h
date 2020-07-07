#pragma once

struct Material
{
    Vec3 diffuse;
    Vec3 specular;
    Vec3 reflection;
    float shine;
};

struct Ray
{
    Vec3 o;
    Vec3 d;
};

struct Hit
{
    float t;
    Ray ray;
    Vec3 normal;
    Material mat;
};

inline Material MATERIAL(Vec3 diffuse, Vec3 specular, Vec3 reflection, float shine);
inline Ray RAY(Vec3 o, Vec3 d);
bool WorldHitGeometry(Ray ray, Hit *hit);
Vec3 WorldHit(Ray ray, bool nolight = false, int n = 1);
