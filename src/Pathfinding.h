#pragma once
#include "World.h"
#include "Node.h"
#include "VectorXYZ.h"
#include <cmath>
#include <iostream>

unsigned int distance(VectorXYZ& startPos, VectorXYZ& endPos) {
	return std::abs(startPos.x - endPos.x) + std::abs(startPos.y - endPos.y) + std::abs(startPos.z - endPos.z);
}

Node* selectNode(std::vector<Node*>& nodes, Node &n) {
	for (int i = 0; i < nodes.size(); i++) {
		if (*nodes[i] == n) {
			return nodes[i];
		}
	}

	return nullptr;
}

bool hasNode(std::vector<Node*>& nodes, Node &n) {
	for (int i = 0; i < nodes.size(); i++) {
		if (*nodes[i] == n) {
			return true;
		}
	}

	return false;
}

bool sufficientHeight(World* w, int x, int y, int z, unsigned int height) {
	for (unsigned int i = 0; i <= height; i++) {
		if (w->getBlock(x, y - 1 - i, z)) {
			return true;
		}
	}
	return false;
}

bool isValid(World* w,Node* n, int x, int y, int z, bool gravity = false, unsigned int jumpHeight = 1, unsigned int maxFall = 3) {
	
	int dx = x - n->x;
	int dy = y - n->y;
	int dz = z - n->z;

	if (dx == 0 && dy == 0 && dz == 0) {
		return false;
	}

	bool result = (!w->getBlock(n->x + dx, n->y, n->z) && !w->getBlock(n->x, n->y + dy, n->z) && !w->getBlock(n->x, n->y, n->z + dz));
	if (!gravity || !result) {
		return result;
	}
	
	int sum = std::abs(dx) + std::abs(dy) + std::abs(dz);
	if (sum == 3) {
		return false;
	}
	else if (sum == 2 && dy != 0) {
		return false;
	}

	bool grounded = w->getBlock(n->x, n->y - 1, n->z);
	bool targetGrounded = w->getBlock(x, y - 1, z);

	if (!grounded) {
		if (!targetGrounded) {
			if (dy == 0) {
				return false;
			}
			else if (dy == 1) {
				return sufficientHeight(w, x, y, z, jumpHeight);
			}
			else {
				return sufficientHeight(w, n->x, n->y, n->z, maxFall);
			}
		}
		else {
			if (dy == -1) {
				return true;
			}
			else {
				return sufficientHeight(w, n->x, n->y, n->z, jumpHeight);
			}
		}
	}

	if (!targetGrounded) {
		if (dy == 1) {
			return sufficientHeight(w, x, y, z, jumpHeight);
		}
		else {
			return sufficientHeight(w, x, y, z, maxFall);
		}
	}

	return true;
}



Node* aStar(World& world, VectorXYZ& startPos, VectorXYZ& endPos, unsigned int jumpHeight, unsigned int maxFall) {
	Node* startNode = new Node(startPos.x, startPos.y, startPos.z, nullptr, 0, distance(startPos, endPos));
	Node* endNode = new Node(endPos.x, endPos.y, endPos.z, nullptr, 100000, 0);
	
	if (world.getBlock(endPos.x, endPos.y, endPos.z)) {
		delete startNode;
		return endNode;
	}

	std::vector<Node*> openedNodes = std::vector<Node*>();
	openedNodes.push_back(startNode);
	std::vector<Node*> closedNodes = std::vector<Node*>();

	int count = 0;

	while (openedNodes.size()) {

		//Select node with the lowest 
		std::sort(std::begin(openedNodes), std::end(openedNodes),
			[](const auto& lhs, const auto& rhs) {
			return lhs->f > rhs->f;
		});
		Node* node = openedNodes.back();
		
		openedNodes.pop_back();

		if (*node == *endNode) {
			delete endNode;
			endNode = node;
			break;
		}
		if (++count > 10000) {
			break;
		}

		closedNodes.push_back(node);
		std::vector<Node*> adjacentNodes = std::vector<Node*>();
		int x = node->x;
		int y = node->y;
		int z = node->z;

		bool gravity = true;

		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				for (int dz = -1; dz <= 1; dz++) {
					if (isValid(&world, node, x + dx, y + dy, z + dz, gravity, jumpHeight, maxFall))
						adjacentNodes.push_back(new Node(x + dx, y + dy, z + dz));
				}
			}
		}
		

		for (int i = 0; i < adjacentNodes.size(); i++) {
			Node* n = adjacentNodes[i];
			if (!hasNode(closedNodes, *n)) {
				if (!hasNode(openedNodes, *n)) {
					openedNodes.push_back(n);
					VectorXYZ pos = VectorXYZ(n->x, n->y, n->z);
					n->g = distance(pos, endPos);
					n->f = n->h + n->g;
				}
				else {
					Node* new_node = selectNode(openedNodes, *n);
					delete n;
					n = new_node;

				}
				float distance = 1.0;
				int dx = n->x - x;
				int dy = n->y - y;
				int dz = n->z - z;
				int sum = std::abs(dx) + std::abs(dy) + std::abs(dz);
				if (sum == 2) {
					distance = 1.4;
				}
				else if (sum == 3) {
					distance = 1.7;
				}

				if (n->h > node->h + distance) {
					n->h = node->h + distance;
					n->f = n->h + n->g;
					n->p = node;
				}
			}
			else {
				delete n;
			}
		}

	}

	std::vector<Node*> finalNodes = std::vector<Node*>();
	Node* node = endNode;
	while (node) {
		finalNodes.push_back(node);
		node = node->p;
	}

	for (int i = 0; i < openedNodes.size(); i++) {
		Node* n = openedNodes[i];
		if (!hasNode(finalNodes, *n)) {
			delete n;
		}
	}

	for (int i = 0; i < closedNodes.size(); i++) {
		Node* n = closedNodes[i];
		if (!hasNode(finalNodes, *n)) {
			delete n;
		}
	}
	std::cout << "Path distance: " << endNode->h  << std::endl;
	return endNode;
}