#include "raytracer.h"

inline Material MATERIAL(Vec3 diffuse, Vec3 specular, Vec3 reflection, float shine)
{
    Material result;
    result.diffuse = diffuse;
    result.specular = specular;
    result.reflection = reflection;
    result.shine = shine;
    result.mirror = false;
    return result;
}

inline Ray RAY(Vec3 o, Vec3 d)
{
    Ray result;
    result.o = o;
    result.d = d;
    return result;
}

bool WorldHitGeometry(World *world, Ray ray, Hit *hit)
{
    bool isHit = false;
    Hit nextHit;
    Hit closestHit;

    for (int i = 0; i < world->geometryCount; i++)
    {
        bool hitSuccess = false;
        if (world->geometries[i].type == Geo_Sphere)
        {
            hitSuccess = SphereHit(world->spheres[world->geometries[i].id], ray, &nextHit);
        }
        else if (world->geometries[i].type = Geo_Plane)
        {
            hitSuccess = PlaneHit(world->planes[world->geometries[i].id], ray, &nextHit);
        }

        if (hitSuccess)
        {
            if (!isHit)
            {
                isHit = true;
                closestHit = nextHit;
            }
            else if (nextHit.t < closestHit.t)
            {
                closestHit = nextHit;
            }
        }
    }
    *hit = closestHit;
    return isHit;
}

DWORD WINAPI WorldHitParallel(LPVOID lpParameter)
{

    HitData *hitData = (HitData *)lpParameter;
    hitData->world->spheres[0].mat.mirror = true;
    hitData->world->spheres[1].mat.mirror = true;
    hitData->world->spheres[2].mat.mirror = true;
    hitData->world->spheres[3].mat.mirror = true;
    // world->planes[4].mat.mirror = true;
    hitData->world->planes[4].mat.diffuse = VEC3(0.2f, 0.2f, 0.2f);

    hitData->n++;
    Vec3 bgCol = VEC3(0, 0, 0);
    Vec3 ambient = VEC3(0.2f, 0.2f, 0.2f);

    DirLight light = DIRLIGHT(VEC3(0.2f, -1.0f, -0.2f), VEC3(0.3f, 0.3f, 0));

    PointLight pointLight = POINTLIGHT(VEC3(2.0f, 0.5f, 0.0f),
                                       VEC3(0.5f, 0.2f, 0.2f));

    PointLight pointLight2 = POINTLIGHT(VEC3(-2.0f, 0.5f, -5.0f),
                                        VEC3(0.2f, 0.3f, 0.6f));

    Hit hit;
    if (WorldHitGeometry(hitData->world, hitData->ray, &hit))
    {
        if (hitData->nolight)
        {
            *hitData->outColor = hit.mat.diffuse;
            return 0;
        }

        Vec3 p = hitData->ray.o + (hitData->ray.d * hit.t);

        Vec3 result = hit.mat.diffuse * ambient;
        if (PointLightIlluminates(hitData->world, pointLight, p))
        {
            result += LightGetColor(Norm(light.dir * -1), light.color, hit);
            Vec3 pointFrom = Norm(pointLight.pos - p);
            result += LightGetColor(pointFrom, pointLight.color, hit);
        }

        if (PointLightIlluminates(hitData->world, pointLight2, p))
        {
            result += LightGetColor(Norm(light.dir * -1), light.color, hit);
            Vec3 pointFrom = Norm(pointLight2.pos - p);
            result += LightGetColor(pointFrom, pointLight2.color, hit);
        }

        if (hit.mat.mirror)
        {
            if (hitData->n < 100)
            {
                Vec3 reflectionDir = Norm(Reflect(hit.ray.o - p, hit.normal));
                Vec3 adjustedP = p + (reflectionDir * EPSILON);
                Ray reflectionRay;
                reflectionRay.o = adjustedP;
                reflectionRay.d = reflectionDir;
                result += hit.mat.reflection * WorldHit(hitData->world, reflectionRay, false, hitData->n);
            }
        }

        *hitData->outColor = result;
        // return result;
    }

    *hitData->outColor = bgCol;
    return 0;
    // return bgCol;
}

Vec3 WorldHit(World *world, Ray ray, bool nolight, int n)
{

    world->spheres[0].mat.mirror = true;
    world->spheres[1].mat.mirror = true;
    world->spheres[2].mat.mirror = true;
    world->spheres[3].mat.mirror = true;
    // world->planes[4].mat.mirror = true;
    world->planes[4].mat.diffuse = VEC3(0.2f, 0.2f, 0.2f);

    n++;
    Vec3 bgCol = VEC3(0, 0, 0);
    Vec3 ambient = VEC3(0.2f, 0.2f, 0.2f);

    DirLight light = DIRLIGHT(VEC3(0.2f, -1.0f, -0.2f), VEC3(0.3f, 0.3f, 0));

    PointLight pointLight = POINTLIGHT(VEC3(2.0f, 0.5f, 0.0f),
                                       VEC3(0.5f, 0.2f, 0.2f));

    PointLight pointLight2 = POINTLIGHT(VEC3(-2.0f, 0.5f, -5.0f),
                                        VEC3(0.2f, 0.3f, 0.6f));

    Hit hit;
    if (WorldHitGeometry(world, ray, &hit))
    {
        if (nolight)
            return hit.mat.diffuse;

        Vec3 p = ray.o + (ray.d * hit.t);

        Vec3 result = hit.mat.diffuse * ambient;
        if (PointLightIlluminates(world, pointLight, p))
        {
            result += LightGetColor(Norm(light.dir * -1), light.color, hit);
            Vec3 pointFrom = Norm(pointLight.pos - p);
            result += LightGetColor(pointFrom, pointLight.color, hit);
        }

        if (PointLightIlluminates(world, pointLight2, p))
        {
            result += LightGetColor(Norm(light.dir * -1), light.color, hit);
            Vec3 pointFrom = Norm(pointLight2.pos - p);
            result += LightGetColor(pointFrom, pointLight2.color, hit);
        }

        if (hit.mat.mirror)
        {
            if (n < 100)
            {
                Vec3 reflectionDir = Norm(Reflect(hit.ray.o - p, hit.normal));
                Vec3 adjustedP = p + (reflectionDir * EPSILON);
                Ray reflectionRay;
                reflectionRay.o = adjustedP;
                reflectionRay.d = reflectionDir;
                result += hit.mat.reflection * WorldHit(world, reflectionRay, false, n);
            }
        }

        return result;
    }

    return bgCol;
}
