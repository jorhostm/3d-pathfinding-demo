#include "Ray.h"

Ray::Ray(glm::vec3 &pos, glm::vec3 &dir)
{
	position = glm::vec3(pos.x, pos.y, pos.z);
	direction = glm::vec3(dir.x, dir.y, dir.z);
}
void Ray::step(float t) {
	position += t * direction;
}
