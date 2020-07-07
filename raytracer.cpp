#include "raytracer.h"

inline Material MATERIAL(Vec3 diffuse, Vec3 specular, Vec3 reflection, float shine)
{
    Material result;
    result.diffuse = diffuse;
    result.specular = specular;
    result.reflection = reflection;
    result.shine = shine;
    return result;
}

inline Ray RAY(Vec3 o, Vec3 d)
{
    Ray result;
    result.o = o;
    result.d = d;
    return result;
}

bool WorldHitGeometry(Ray ray, Hit *hit)
{
    Material mat1 = MATERIAL(VEC3(0.5f, 0.2f, 0.2f), 
			     VEC3(0.8f, 0.2f, 0.1f), 
			     VEC3(0.2f, 0.2f, 0.2f), 
			     6.0f);
    Material mat2 = MATERIAL(VEC3(0.2f, 0.8f, 0.0f), 
			     VEC3(0.3f, 0.3f, 0.3f), 
			     VEC3(0.1f, 0.1f, 0.1f), 
			     15.0f);
    Material mat3 = MATERIAL(VEC3(0.0f, 0.0f, 1.0f), 
			     VEC3(0.6f, 0.6f, 0.6f), 
			     VEC3(0.5f, 0.5f, 0.5f), 
			     15.0f);
    Sphere spheres[] = {
	SPHERE(VEC3(-1.0f, 0, -3.0f), 0.5f, mat2),
	SPHERE(VEC3(1.0f, 0, -3.0f), 0.5f, mat1)
    };
    Plane planes[] = {
	// PLANE(VEC3(-3, 0, 0), VEC3(1,0,0), mat3),
	// PLANE(VEC3(3, 0, 0), VEC3(-1,0,0), mat3),
	PLANE(VEC3(0, -1.5f, 0), VEC3(0,1,0), mat3),
	// PLANE(VEC3(0, 1.5f, 0), VEC3(0,-1,0), mat3),
	// PLANE(VEC3(0, 0, -6), VEC3(0, 0, 1), mat3),
    };

    bool isHit = false;
    Hit nextHit;
    Hit closestHit;

    for(int i = 0; i < ARRAY_SIZE(spheres); i++) {
    	if(SphereHit(spheres[i], ray, &nextHit)) {
	    if(!isHit) {
		isHit = true;
    		closestHit = nextHit;
	    }
    	    else if(nextHit.t < closestHit.t) {
    		closestHit = nextHit;
    	    }
    	}
    }

    for (int i = 0; i < ARRAY_SIZE(planes); ++i) {
	if(PlaneHit(planes[i], ray, &nextHit)) {
	    if(!isHit) {
		isHit = true;
    		closestHit = nextHit;
	    }
    	    else if(nextHit.t < closestHit.t) {
    		closestHit = nextHit;
    	    }
	}
    }

    *hit = closestHit;
    return isHit;
}

Vec3 WorldHit(Ray ray, bool nolight, int n)
{
    n++;
    Vec3 bgCol = VEC3(0,0,0);
    Vec3 ambient = VEC3(0.2f, 0.2f, 0.2f);
    
    DirLight light = DIRLIGHT(VEC3(0.2f,-1.0f,-0.2f), VEC3(0.3f, 0.3f, 0));
    
    PointLight pointLight = POINTLIGHT(VEC3(2.0f, 0.5f, -2.0f), 
				       VEC3(0.7f, 0.7f, 0.7f));
    
    Hit hit;
    if(WorldHitGeometry(ray, &hit)) {
	if(nolight)
	    return hit.mat.diffuse;
	
	Vec3 p = ray.o + (ray.d * hit.t);

	Vec3 result = hit.mat.diffuse * ambient;
	if(PointLightIlluminates(pointLight, p)) {
	    result += LightGetColor(Norm(light.dir * -1), light.color, hit);
	    Vec3 pointFrom = Norm(pointLight.pos - p);
	    result += LightGetColor(pointFrom, pointLight.color, hit);
	}
	
	if(n < 10) {
	    Vec3 reflectionDir = Norm(Reflect(hit.ray.o - p, hit.normal));
	    Vec3 adjustedP = p + (reflectionDir * EPSILON);
	    Ray reflectionRay;
	    reflectionRay.o = adjustedP;
	    reflectionRay.d = reflectionDir;
	    result += hit.mat.reflection * WorldHit(reflectionRay, false, n);
	}

	return result;
    }

    return bgCol;
}
