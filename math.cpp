#include <math.h>

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

#define PI 3.14159265358979323846264338327950288
#define RADIANS(degrees) ((degrees) * PI / 180.0)
#define DEGREES(radians) ((radians) * 180.0 / PI)

union Vec3
{
    struct {
	float x, y, z;
    };
    struct {
	float r, g, b;
    };
};

struct Mat3x3
{
// ROW MAJOR [ROW][COLUMN]
    float e[3][3];
};

inline Vec3 vec3(float x, float y, float z)
{
    Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

inline Vec3 operator+(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

inline Vec3 &operator+=(Vec3 &a, Vec3 b)
{
    a = a + b;
    return a;
}

inline Vec3 operator-(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

inline Vec3 &operator-=(Vec3 &a, Vec3 b)
{
    a = a - b;
    return a;
}

inline Vec3 operator*(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

inline Vec3 &operator*=(Vec3 &a, Vec3 b)
{
    a = a * b;
    return a;
}

inline Vec3 operator*(Vec3 a, float s)
{
    Vec3 result;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
    return result;
}

inline Vec3 &operator*=(Vec3 &a, float s)
{
    a = a * s;
    return a;
}

inline Vec3 operator/(Vec3 a, float s)
{
    Vec3 result;
    result.x = a.x / s;
    result.y = a.y / s;
    result.z = a.z / s;
    return result;
}

inline Vec3 &operator/=(Vec3 &a, float s)
{
    a = a / s;
    return a;
}

inline float Square(float a)
{
    return a * a;
}

inline float Length(Vec3 a)
{
    return sqrt(Square(a.x) + Square(a.y) + Square(a.z));
}

inline Vec3 Norm(Vec3 a)
{
    return a / Length(a);
}

inline float Dot(Vec3 a, Vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline Vec3 Cross(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

inline Vec3 Reflect(Vec3 v, Vec3 n)
{
    Vec3 nn = n * (Dot(v, n) * 2);
    return vec3(nn.x - v.x, nn.y - v.y, nn.z - v.z);
}
