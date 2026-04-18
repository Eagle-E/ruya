#include "transform.h"




/*
* Adds xyzDegrees to the current rotation of the object. In other words:
* the object gets rotated further. For accumulating rotations.
*/
void ruya::math::Transform::rotate(float xDegrees, float yDegrees, float zDegrees)
{
    rotation.x = fmod(rotation.x + xDegrees, 360);
    rotation.y = fmod(rotation.y + yDegrees, 360);
    rotation.z = fmod(rotation.z + zDegrees, 360);
}