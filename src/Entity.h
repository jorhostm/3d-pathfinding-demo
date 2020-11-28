#pragma once
#include <glm/glm.hpp>
#include "World.h"
class Entity
{
public:
	World* world;
	glm::vec3 position;
	glm::vec3 dimensions;
	glm::vec3 acceleration;
	glm::vec3 velocity = glm::vec3(0.0,0.0,0.0);
	glm::vec2 direction = glm::vec2(1.0, 0.0);
	float jumpspeeds[5] = { 0.0,7.0,8.9, 11.0, 12.5 };
	float speed = 0.0;
	float acc = 0.0;
	float g = 18;
	float maxSpeed = 4.1;
	int jumpHeight = 1;
	float deceleration = 15.0;
	bool isGrounded = false;
	bool isMoving = false;
	bool isFlying = false;
	unsigned int vao;

	Entity(World* w,glm::vec3 &dim, glm::vec3 pos, float maxSpeed, int jumpHeight);
	~Entity() = default;
	void Collide(float deltaTime);
	void render(glm::mat4 &mPV);
	void move(glm::vec2& dir);
	void stop();
	void jump();
	void crouch();
	void update(float deltaTime);
};

