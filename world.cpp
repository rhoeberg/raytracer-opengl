
void AddPlane(World *world, Plane plane)
{
    if(world->planeCount < world->maxPlanes)
    {
        world->geometries[world->geometryCount].id = world->planeCount;
        world->geometries[world->geometryCount].type = Geo_Plane;
        world->planes[world->planeCount] = plane;
        world->geometryCount++;
        world->planeCount++;
    }
}

void AddSphere(World *world, Sphere sphere)
{
    if(world->sphereCount < world->maxSpheres)
    {
        world->geometries[world->geometryCount].id = world->sphereCount;
        world->geometries[world->geometryCount].type = Geo_Sphere;
        world->spheres[world->sphereCount] = sphere;
        world->geometryCount++;
        world->sphereCount++;
    }
}
