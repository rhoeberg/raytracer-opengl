#include "camera.h"
#include "raytracer.h"
#include <math.h>

inline Camera CAMERA(Vec3 e, Vec3 g, Vec3 t, float angle)
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

Ray RayFor(Camera cam, int w, int h, int x, int y)
{
    Vec3 o = cam.e;
    Vec3 rw = (cam.w * -1) * (h / 2) / tan(cam.angle / 2);
    Vec3 ru = cam.u * (x - (w - 1) / 2);
    Vec3 rv = cam.v * (y - (h - 1) / 2);
    Vec3 r = rw + ru + rv;
    Vec3 d = Norm(r);
    Ray result = RAY(o, d);
    return result;
}
