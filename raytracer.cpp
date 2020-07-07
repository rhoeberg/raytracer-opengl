inline DirLight DIRLIGHT(Vec3 dir, Vec3 color)
{
    DirLight result;
    result.dir = dir;
    result.color = color;
    return result;
}

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
    Material mat1 = MATERIAL(VEC3(0.2f, 0.2f, 0.2f), 
			     VEC3(0.1f, 0.1f, 0.1f), 
			     VEC3(0.8f, 0.8f, 0.8f), 
			     6.0f);
    Material mat2 = MATERIAL(VEC3(1.0f, 1.0f, 0.0f), 
			     VEC3(0, 0, 0), 
			     VEC3(0, 0, 0), 
			     15.0f);
    Material mat3 = MATERIAL(VEC3(0.0f, 0.0f, 1.0f), 
			     VEC3(0.6f, 0.6f, 0.6f), 
			     VEC3(1, 1, 1), 
			     15.0f);
    Sphere spheres[] = {
	SPHERE(VEC3(-1.0f, 0, -3.0f), 0.5f, mat2),
	SPHERE(VEC3(1.0f, 0, -3.0f), 0.5f, mat1)
    };
    Plane plane1 = PLANE(VEC3(0,-1,0), VEC3(0,1,0), mat3);

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

    if(PlaneHit(plane1, ray, &nextHit)) {
    	if(isHit) {
    	    if(nextHit.t < closestHit.t) {
    		closestHit = nextHit;
    	    }
    	}
    	else {
	    isHit = true;
    	    closestHit = nextHit;
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
    DirLight light = DIRLIGHT(VEC3(0.2f,-0.5f,-0.5f), VEC3(1,1,1));

    // bool isHit = false;
    // Hit closestHit;
    // Hit hit;

    // for(int i = 0; i < ARRAY_SIZE(spheres); i++) {
    // 	if(SphereHit(spheres[i], ray, &hit)) {
    // 	    isHit = true;
    // 	    if(i == 0) {
    // 		closestHit = hit;
    // 	    }
    // 	    else if(hit.t < closestHit.t) {
    // 		closestHit = hit;
    // 	    }
    // 	}
    // }

    // if(PlaneHit(plane1, ray, &hit)) {
    // 	if(isHit) {
    // 	    if(hit.t < closestHit.t) {
    // 		closestHit = hit;
    // 	    }
    // 	}
    // 	else {
    // 	    isHit = true;
    // 	    closestHit = hit;
    // 	}
    // }
    
    Hit hit;
    bool isHit = WorldHitGeometry(ray, &hit);

    if(isHit) {
	if(nolight)
	    return hit.mat.diffuse;
	
	Vec3 p = ray.o + (ray.d * hit.t);


	
	Vec3 finalColor = hit.mat.diffuse * ambient;
	Vec3 col = hit.mat.diffuse * light.color;
	Vec3 l = Norm(light.dir * -1);
	float nl = Dot(hit.normal, l);
	col = col * MAX(0.0f, nl);
	
	Vec3 col2 = hit.mat.specular * light.color;
	Vec3 e = Norm(ray.o - p);
	Vec3 rl = Reflect(l, hit.normal);
	float rle = Dot(e, rl);
	col2 *= pow(MAX(0.0f, rle), hit.mat.shine);
	

	finalColor += col + col2;

	if(n < 100) {
	    Vec3 reflectionDir = Norm(Reflect(ray.o - p, hit.normal));
	    Vec3 adjustedP = p + (reflectionDir * EPSILON);
	    Ray reflectionRay;
	    reflectionRay.o = adjustedP;
	    reflectionRay.d = reflectionDir;
	    finalColor += hit.mat.reflection * WorldHit(reflectionRay, false, n);
	}

	return finalColor;
    }

    return bgCol;
};
