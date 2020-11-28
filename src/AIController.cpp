#include "AIController.h"
#include <glm/glm.hpp>
#include <cmath>

AIController::AIController(Entity* entity, Node* path) {
	this->entity = entity;
	this->path = path;
	while (path) {
		nodes.push_back(path);
		path = path->p;
	}
	index = nodes.size() - 1;
}

void AIController::update() {
	if (index < 0) {
		entity->stop();
		return;
	}
	Node* goal = nodes[index];
	int dx = goal->x - std::floor(entity->position.x);
	int dy = goal->y - std::floor(entity->position.y);
	int dz = goal->z - std::floor(entity->position.z);
	//glm::vec3 entityBlockPos = glm::vec3(std::floor(entity->position.x), std::floor(entity->position.y), std::floor(entity->position.y));
	float ddx = goal->x + 0.5 - entity->position.x;
	float ddy = goal->y + 0.5 - entity->position.y;
	float ddz = goal->z + 0.5 - entity->position.z;
	if (dy > 0) {
		entity->jump();
	}

	if (std::abs(ddx) <= 0.1 && std::abs(ddz) <= 0.1) {
		if (std::abs(ddy) <= 0.5) {
			index--;
		}
		entity->stop();

	}
	else if (std::abs(ddy) > 0.5) {
		entity->stop();
	}
	else {
		glm::vec2 dir = glm::normalize(glm::vec2(ddx, ddz));
		entity->move(dir);
	}
}

void AIController::resetPath(Node* path) {
	this->path = path;
	nodes.clear();
	while (path) {
		nodes.push_back(path);
		path = path->p;
	}
	index = nodes.size() - 1;
	if (nodes.size() == 1) {
		index = -1;
	}
}