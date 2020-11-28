#pragma once
#include "Entity.h"
#include "Node.h"
class AIController
{
	Entity* entity;
	Node* path;
	std::vector<Node*> nodes = std::vector<Node*>();
	int index;
public:
	AIController(Entity* entity, Node* path);
	~AIController() = default;
	void update();
	void resetPath(Node* path);
};

