struct DirLight
{
    Vec3 dir;
    Vec3 color;
};

struct Material
{
    Vec3 diffuse;
    Vec3 specular;
    float shine;
};

struct Ray
{
    Vec3 o;
    Vec3 d;
};

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

struct Hit
{
    float t;
    Ray ray;
    Sphere geo;
    Vec3 normal;
    Material mat;
};

struct Camera
{
    Vec3 e;
    Vec3 g;
    Vec3 t;
    
    Vec3 w;
    Vec3 u;
    Vec3 v;
    
    float angle;
};

inline DirLight dirLight(Vec3 dir, Vec3 color)
{
    DirLight result;
    result.dir = dir;
    result.color = color;
    return result;
}

inline Material material(Vec3 diffuse, Vec3 specular, float shine)
{
    Material result;
    result.diffuse = diffuse;
    result.specular = specular;
    result.shine = shine;
    return result;
}

inline Ray ray(Vec3 o, Vec3 d)
{
    Ray result;
    result.o = o;
    result.d = d;
    return result;
}

inline Plane plane(Vec3 a, Vec3 n, Material mat)
{
    Plane result;
    result.a = a;
    result.n = n;
    result.mat = mat;
    return result;
}

inline Sphere sphere(Vec3 c, float r, Material mat)
{
    Sphere result;
    result.c = c;
    result.r = r;
    result.mat = mat;
    return result;
}

inline Camera camera(Vec3 e, Vec3 g, Vec3 t, float angle)
{
    Camera result;
    result.e = e;
    result.g = g;
    result.t = t;
    result.angle = angle;

    result.w = Norm(g * -1);
    result.u = Norm(Cross(t, result.w));
    result.v = Cross(result.w, result.u);
    return result;
}

bool SphereHit(Sphere sphere, Ray ray, Hit *hit)
{
    float a = Dot(ray.d, ray.d);
    float b = Dot(ray.d, (ray.o - sphere.c) * 2);
    float c = Dot(ray.o - sphere.c, ray.o - sphere.c) - Square(sphere.r);
    
    float t = 0;
    float d = Square(b) - 4 * a * c;
    if(d < 0) {
	return false;
    }
    else if(d == 0) {
	t = -0.5 * b / a;
    }
    else {
	float tNeg = (-b - sqrt(d)) / (2 * a);
	float tPos = (-b + sqrt(d)) / (2 * a);
	t = MIN(tNeg, tPos);
    }
    
    if(t < 0) {
	return false;
    }
    
    Vec3 normal = Norm((ray.o + (ray.d * t)) - sphere.c);

    hit->t = t;
    hit->ray = ray;
    hit->geo = sphere;
    hit->normal = normal;
    hit->mat = sphere.mat;

    return true;
}

bool PlaneHit(Plane plane, Ray ray, Hit *hit)
{
    float denom = Dot(plane.n, ray.d);
    if(abs(denom) > EPSILON) {
	float t = Dot(plane.a - ray.o, plane.n) / denom;
	if(t >= 0) {
	    hit->t = t;
	    hit->ray = ray;
	    hit->normal = plane.n;
	    hit->mat = plane.mat;
	    return true;
	}
    }
    return false;
}

Ray RayFor(Camera cam, int w, int h, int x, int y)
{
    Vec3 o = cam.e;
    Vec3 rw = (cam.w * -1) * (h / 2) / tan(cam.angle / 2);
    Vec3 ru = cam.u * (x - (w - 1) / 2);
    Vec3 rv = cam.v * (y - (h - 1) / 2);
    Vec3 r = rw + ru + rv;
    Vec3 d = Norm(r);
    Ray result = ray(o, d);
    return result;
}

bool WorldHitGeometry(Ray ray, Hit *hit)
{
    Material mat1 = material(vec3(1.0f, 0.0f, 0.0f), vec3(0.2f, 0.7f, 0.7f), 15.0f);
    Material mat2 = material(vec3(1.0f, 1.0f, 0.0f), vec3(0, 0, 0), 15.0f);
    Material mat3 = material(vec3(0.0f, 0.0f, 1.0f), vec3(1, 1, 1), 15.0f);
    Sphere spheres[] = {
	sphere(vec3(1, 0, -3.0f), 0.5f, mat1),
	sphere(vec3(-1, 0, -3.0f), 0.5f, mat2)
    };
    Plane plane1 = plane(vec3(0,-1,0), vec3(0,1,0), mat3);

    bool isHit = false;
    // Hit closestHit;

    Hit nextHit;
    Hit closestHit;
    for(int i = 0; i < ARRAY_SIZE(spheres); i++) {
    	if(SphereHit(spheres[i], ray, &nextHit)) {
    	    isHit = true;
    	    if(i == 0) {
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

Vec3 WorldHit(Ray ray, bool nolight = false)
{
    Vec3 bgCol = vec3(0,0,0);
    Vec3 ambient = vec3(0.2f, 0.2f, 0.2f);
    DirLight light = dirLight(vec3(0,-1,0), vec3(1,1,1));

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
	return finalColor;
    }

    return bgCol;
};
