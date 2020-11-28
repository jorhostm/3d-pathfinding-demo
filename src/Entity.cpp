#include "Entity.h"
#include "Quad.h"
#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include "Utils.h"


Entity::Entity(World* w, glm::vec3& dim, glm::vec3 pos, float maxSpeed, int jumpHeight)
{
    world = w;
    dimensions = glm::vec3(dim.x, dim.y, dim.z);
    position = glm::vec3(pos.x, pos.y, pos.z);
    acceleration = glm::vec3(0.0, -g, 0.0);
    this->maxSpeed = maxSpeed;
    this->jumpHeight = jumpHeight;
    std::vector<float> meshdata = std::vector<float>();
    std::vector<float> colour = {1.0,1.0,1.0,1.0};
    
    float p[3] = { -0.5,-0.5,-0.5 };
    for (int i = 0; i < 6; i++) {
        Quad::addQuad(meshdata, i, p, colour);
    }
    unsigned int vbo;
    Utils::create_vbo(meshdata, &vao, &vbo);
}


void Entity::Collide(float deltaTime)
{
    // x-direction:
    float dx = dimensions.x;
    if (velocity.x < 0.0) {
        dx = -dx;
    }
    for (float z = -dimensions.z; z <= dimensions.z; z += 2 * dimensions.z) {
        for (float y = -dimensions.y; y <= dimensions.y; y += 2 * dimensions.y) {
            glm::vec3 pos = glm::vec3(position.x + dx , position.y + y , position.z + z);
            glm::vec3 newPos = glm::vec3(pos.x + velocity.x * deltaTime, pos.y, pos.z );
            if (std::floor(pos.x) != std::floor(newPos.x) || std::floor(pos.y) != std::floor(newPos.y) || std::floor(pos.z) != std::floor(newPos.z)) {
                if (world->getBlock(newPos.x, newPos.y, newPos.z)) {
                    velocity.x = 0.0;
                }
            }
        }
    }

    // z-direction:
    float dz = dimensions.z;
    if (velocity.z < 0.0) {
        dz = -dz;
    }
    for (float x = -dimensions.x; x <= dimensions.x; x += 2 * dimensions.x) {
        for (float y = -dimensions.y; y <= dimensions.y; y += 2 * dimensions.y) {
            glm::vec3 pos = glm::vec3(position.x + x, position.y + y, position.z + dz);
            glm::vec3 newPos = glm::vec3(pos.x, pos.y , pos.z + velocity.z * deltaTime);
            if (std::floor(pos.x) != std::floor(newPos.x) || std::floor(pos.y) != std::floor(newPos.y) || std::floor(pos.z) != std::floor(newPos.z)) {
                if (world->getBlock(newPos.x, newPos.y, newPos.z)) {
                    velocity.z = 0.0;
                }
            }
        }
    }

    // y-direction:
    float dy = dimensions.y;
    if (velocity.y < 0.0) {
        dy = -dy;
    }
    for (float x = -dimensions.x; x <= dimensions.x; x += 2 * dimensions.x) {
        for (float z = -dimensions.z; z <= dimensions.z; z += 2 * dimensions.z) {
            glm::vec3 pos = glm::vec3(position.x + x, position.y + dy, position.z + z);
            glm::vec3 newPos = glm::vec3(pos.x, pos.y + velocity.y * deltaTime, pos.z );
            if (std::floor(pos.x) != std::floor(newPos.x) || std::floor(pos.y) != std::floor(newPos.y) || std::floor(pos.z) != std::floor(newPos.z)) {
                if (world->getBlock(newPos.x, newPos.y, newPos.z)) {
                    velocity.y = 0.0;
                    if (dy < 0.0) {
                        isGrounded = true;
                    }
                }
                else {
                    isGrounded = false;
                }
            }
        }
    }

}

void Entity::render(glm::mat4 &mPV)
{
    glm::mat4 mModel = glm::identity<glm::mat4>();
    
    mModel = glm::translate(mModel, position);
    glm::vec3 dim = glm::vec3(2*dimensions.x, 2*dimensions.y, 2*dimensions.z);
    mModel = glm::scale(mModel, dim);
    glm::mat4 mPVM = mPV * mModel;
    glUniformMatrix4fv(4, 1, GL_FALSE, &mPVM[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_QUADS, 0, 32);
}

void Entity::move(glm::vec2& dir)
{
    isMoving = true;
    direction.x = dir.x;
    direction.y = dir.y;

    acc = 20.0;
    
}

void Entity::jump()
{
    if (!isFlying) {
        if (isGrounded) {
            isGrounded = false;
            velocity.y += jumpspeeds[jumpHeight];
        }
    }
    else {
        velocity.y = maxSpeed;
    }
}

void Entity::update(float deltaTime) {
    speed += acc * deltaTime;
    if (speed > maxSpeed) {
        speed = maxSpeed;
    }
    else if (speed < 0.0) {
        speed = 0.0;
    }
    velocity.x = speed * direction.x;
    velocity.z = speed * direction.y;
    if(!isFlying)
        velocity.y += acceleration.y * deltaTime;

   

    Collide(deltaTime);
    
    position += velocity * deltaTime;

    if (isFlying) {
        velocity.y = 0;
    }

    
}

void Entity::crouch() {
    if (isFlying)
        velocity.y = -maxSpeed;
}

void Entity::stop() {
    isMoving = false;
    acc = -deceleration;
    
}
