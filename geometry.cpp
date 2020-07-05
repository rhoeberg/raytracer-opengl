enum GeometryType
{
    PLANE,
    SPHERE
};

struct Geometry
{
    GeometryType type;
    Material material;
};

struct Sphere
{
    Vec3 c;
    float r;
};

inline Sphere sphere(Vec3 c, float r)
{
    Sphere result;
    result.c = c;
    result.r = r;
    return result;
}

Hit SphereHit(Sphere sphere, Ray ray)
{
    float a = Dot(ray.d, ray.d);
    float b = Dot(ray.d, (ray.o - sphere.c) * 2);
    float c = Dot(ray.o - sphere.c, ray.o - sphere.c) - Square(sphere.r);
    
    float t = 0;
    float d = Square(b) - 4 * a * c;
    if(d < 0) {
	return NULL;
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
	return NULL;
    }
    
    Vec3 normal = Norm((r.o + (r.d * t)) - r.c);
    Hit result;
    result.t = t;
    result.r = r;
    result.geo = sphere;
    return result;
}
