#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iterator>
#include <string>
#include <fstream>
#include <iostream>
#include "stb_image.h"
#include "Chunk.h"
#include "World.h"
#include "Cube.h"
#include "Ray.h"
#include "Node.h"
#include "Pathfinding.h"
#include "Entity.h"
#include "AIController.h"

static const int W_WIDTH = 1980;
static const int W_HEIGHT = 1080;

static std::vector<int> keys = std::vector<int>(10);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        keys.push_back(key);
    }
    else if (action == GLFW_RELEASE) {
        std::vector<int>::iterator position = std::find(keys.begin(), keys.end(), key);
        keys.erase(position);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        keys.push_back(button);
    }
    else if (action == GLFW_RELEASE) {
        std::vector<int>::iterator position = std::find(keys.begin(), keys.end(), button);
        keys.erase(position);
    }
}


static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << 
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static unsigned int create_shader_program(const std::string& vertexShader, const std::string& fragmentShader) {
    auto program_id = glCreateProgram();
    
    std::ifstream in(vertexShader.c_str());
    std::string vShader((std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>());


    in = std::ifstream (fragmentShader.c_str());
    std::string fShader((std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>());
  
    return CreateShader(vShader, fShader);
}

static unsigned int CreateTexture(const char *name) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load(name, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void deletePath(Node* n) {
    while (n) {
        Node* parent = n->p;
        delete n;
        n = parent;
    }
}


int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "3D Pathfinding Demo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewInit();
     
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glDisable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    World world = World();
    world.addBlock(0, 0, 0);
    Chunk* c = world.getChunk(0, 0, 0);
    for (int i = 0; i < Chunk::CHUNK_SIZE; i++)
        for (int j = 0; j < Chunk::CHUNK_SIZE; j++)
            for (int k = 0; k < Chunk::CHUNK_SIZE; k++)
                c->addBlock(i, j, k);
    
    unsigned int crossShader = create_shader_program("CrosshairVertex.txt", "CrosshairFragment.txt");

    unsigned int shader = create_shader_program("vertex.txt", "fragment.txt");
    glUseProgram(shader);

    unsigned int texture = CreateTexture("test.jpg");
    glActiveTexture(GL_TEXTURE0);
    

    glm::mat4 mProjection = glm::perspective(45.0f, (float)W_WIDTH / (float) W_HEIGHT, 0.1f, 1000.0f);
    glm::vec3 cameraPos = glm::vec3(0.0);
    glm::vec2 cameraRot = glm::vec2(0.0, 0.0);

    glm::vec3 camera_dir_x = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 camera_dir_y = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 camera_dir_z = glm::vec3(0.0, 0.0, -1.0);
   
    float speed = 10;

    double elapsedTime = 0;
    glfwSetTime(elapsedTime);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    //Countdowns and timers
    double destroyBlockCountdown = 0.0;
    double placeBlockCountdown = 0.0;
    double placeStartCountdown = 0.0;
    double placeEndCountdown = 0.0;
    double calculatePathCountdown = 0.0;
    

    /* Loop until the user closes the window */
    std::vector<float> yellow = { 1.0,1.0,0.0,1.0 };
    std::vector<float> c1 = { 1.0,0.0,1.0,1.0 };
    std::vector<float> c2 = { 0.0,1.0,1.0,1.0 };
    Cube cube = Cube(yellow);
    Cube startCube = Cube(c1);
    Cube endCube = Cube(c2);

    Node startNode = Node(0, 16, 0);
    Node endNode = Node(15, 16, 15);
   
    VectorXYZ start = VectorXYZ(startNode.x, startNode.y, startNode.z);
    VectorXYZ end = VectorXYZ(endNode.x, endNode.y, endNode.z);
    Node* path = aStar(world, start, end, 0, 3);

    //Player Entity
    glm::vec3 playerPos = glm::vec3(7.0, 20, 7.0);
    glm::vec3 PlayerDim = glm::vec3(0.3, 0.85, 0.3);
    Entity player = Entity(&world,PlayerDim, playerPos, 4.17,1);

    //AI Entity
    glm::vec3 entityPos = glm::vec3(7.0, 20, 7.0);
    glm::vec3 entityDim = glm::vec3(0.3, 0.3, 0.3);
    int jumpHeight = 4;
    int maxFall = 16;
    Entity entity = Entity(&world, entityDim, entityPos, 3.0, jumpHeight);

    AIController aicontroller = AIController(&entity, path);

    while (!glfwWindowShouldClose(window))
    {
        
        /* Render here */
        glClearColor(0.163, 0.163, 0.163, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float deltaTime = elapsedTime;
        elapsedTime = glfwGetTime();
        deltaTime = elapsedTime - deltaTime;

        // Mouse movement
        double dxpos = xpos;
        double dypos = ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        dxpos = xpos - dxpos;
        dypos = ypos - dypos;

        cameraRot.y -= 0.5*dxpos*deltaTime;
        cameraRot.x -= 0.5*dypos*deltaTime;

        if (cameraRot.x < -3.14/2) {
            cameraRot.x = -3.14/2;
        }
        else if (cameraRot.x > 3.14/2) {
            cameraRot.x = 3.14/2;
        }


        // Handle Mouse Input
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int ctrlState = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
        int altState = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
        if (state == GLFW_PRESS){
            destroyBlockCountdown -= deltaTime;
            Ray r = Ray(cameraPos, camera_dir_z);
            for (int i = 0; i < 60; i++) {
                if (world.getBlock(r.position.x, r.position.y, r.position.z)) {
                    if (destroyBlockCountdown <= 0.0) {
                        world.removeBlock(r.position.x, r.position.y, r.position.z);
                        destroyBlockCountdown = 0.2;
                        }
                    break;
                }
                r.step(0.1);

            }
        }
        else {
            destroyBlockCountdown = 0.0;
        }

        state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        if (state == GLFW_PRESS) {
            placeBlockCountdown -= deltaTime;
            Ray r = Ray(cameraPos, camera_dir_z);
            for (int i = 0; i <= 60; i++) {
                if (world.getBlock(r.position.x, r.position.y, r.position.z)) {
                    if (placeBlockCountdown <= 0.0) {
                        for (int j = 0; j < 10; j++) {
                            r.step(-0.01);
                            if (!world.getBlock(r.position.x, r.position.y, r.position.z)){
                                if (ctrlState == GLFW_PRESS) {
                                    startNode.x = std::floor(r.position.x);
                                    startNode.y = std::floor(r.position.y);
                                    startNode.z = std::floor(r.position.z);
                                    start = VectorXYZ(startNode.x, startNode.y, startNode.z);
                                }
                                else if (altState == GLFW_PRESS) {
                                    endNode.x = std::floor(r.position.x);
                                    endNode.y = std::floor(r.position.y);
                                    endNode.z = std::floor(r.position.z);
                                    end = VectorXYZ(endNode.x, endNode.y, endNode.z);
                                }
                                else {
                                    world.addBlock(r.position.x, r.position.y, r.position.z);
                                }
                                    placeBlockCountdown = 0.2;
                                    break;
                                
                            }
                        }
                        break;
                    }
                }
                r.step(0.1);

            }
        }
        else {
            placeBlockCountdown = 0.0;
        }
        
        // Entity control stuff
        int entityKeyState[5] = { 0 };
        glm::vec2 dir = glm::vec2(0.0, 0.0);
        entityKeyState[0] = glfwGetKey(window, GLFW_KEY_W);
        entityKeyState[1] = glfwGetKey(window, GLFW_KEY_S);
        entityKeyState[2] = glfwGetKey(window, GLFW_KEY_A);
        entityKeyState[3] = glfwGetKey(window, GLFW_KEY_D);
        
        
        bool stop = true;
        for (int i = 0; i < 4; i++) {
            if (entityKeyState[i] != GLFW_RELEASE) {
                stop = false;
                break;
            }
        }

        // Key input
        for (int key : keys) {
            switch (key) {
            case GLFW_KEY_W:
                cameraPos += speed * deltaTime * camera_dir_z;
                dir.x += camera_dir_z.x;
                dir.y += camera_dir_z.z;
                break;
            case GLFW_KEY_S:
                cameraPos -= speed * deltaTime * camera_dir_z;
                dir.x -= camera_dir_z.x;
                dir.y -= camera_dir_z.z;
                break;
            case GLFW_KEY_A:
                cameraPos -= speed * deltaTime * camera_dir_x;
                dir.x -= camera_dir_x.x;
                dir.y -= camera_dir_x.z;
                break;
            case GLFW_KEY_D:
                cameraPos += speed * deltaTime * camera_dir_x;
                dir.x += camera_dir_x.x;
                dir.y += camera_dir_x.z;
                break;
            case GLFW_KEY_SPACE:
                cameraPos += speed * deltaTime * camera_dir_y;
                player.jump();
                break;
            case GLFW_KEY_LEFT_SHIFT:
                cameraPos -= speed * deltaTime*camera_dir_y;
                player.crouch();
                break;

            case GLFW_KEY_R:
                player.position.x = 7;
                player.position.y = 17;
                player.position.z = 7;
                cameraRot.x = 0.0;
                cameraRot.y = 0.0;
                break;

            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_E:
                placeStartCountdown -= deltaTime;
                if (placeStartCountdown <= 0.0) {
                    startNode.x = std::floor(cameraPos.x);
                    startNode.y = std::floor(cameraPos.y);
                    startNode.z = std::floor(cameraPos.z);
                    start = VectorXYZ(startNode.x, startNode.y, startNode.z);
                    placeStartCountdown = 0.2;
                }
                break;

            case GLFW_KEY_Q:
                placeEndCountdown -= deltaTime;
                if (placeEndCountdown <= 0.0) {
                    endNode.x = std::floor(cameraPos.x);
                    endNode.y = std::floor(cameraPos.y);
                    endNode.z = std::floor(cameraPos.z);
                    end = VectorXYZ(endNode.x, endNode.y, endNode.z);
                    placeEndCountdown = 0.2;
                }
                break;

            case GLFW_KEY_ENTER:
                calculatePathCountdown -= deltaTime;
                if (calculatePathCountdown <= 0.0) {
                    deletePath(path);

                    path = aStar(world, start, end, jumpHeight, maxFall);
                    entity.position.x = start.x + 0.5;
                    entity.position.y = start.y + 1.0;
                    entity.position.z = start.z + 0.5;
                    entity.jumpHeight = jumpHeight;
                    aicontroller.resetPath(path);
                    calculatePathCountdown = 0.2;
                }
                break;

            case GLFW_KEY_J:
                if (player.isFlying == false) {
                    player.maxSpeed = player.maxSpeed * 2.0;
                    player.isFlying = true;
                }
                break;
            case GLFW_KEY_K:
                if(player.isFlying == true){
                    player.maxSpeed = player.maxSpeed / 2.0;
                    player.isFlying = false;
                }
                break;

            case GLFW_KEY_0:
                jumpHeight = 0;
                break;
            case GLFW_KEY_1:
                jumpHeight = 1;
                break;
            case GLFW_KEY_2:
                jumpHeight = 2;
                break;
            case GLFW_KEY_3:
                jumpHeight = 3;
                break;
            case GLFW_KEY_4:
                jumpHeight = 4;
                break;
            
            }

        }

        if (stop || (dir.x == 0.0 && dir.y == 0.0)) {
            player.stop();
        }
        else {

            dir = glm::normalize(dir);
            player.move(dir);
        }
        player.update(deltaTime);
        cameraPos = player.position;
        cameraPos.y += PlayerDim.y - 0.1;

        aicontroller.update();
        entity.update(deltaTime);


        // Construct a transformation matrix
        glm::mat4 mView = glm::identity<glm::mat4>();

        // Apply each rotation along every major axis
        mView = glm::rotate(mView, -cameraRot.x, glm::vec3(1.0, 0.0, 0.0));
        mView = glm::rotate(mView, -cameraRot.y, glm::vec3(0.0, 1.0, 0.0));
        

        // Rotate the camera direction's relative axis
        camera_dir_x = glm::inverse(mView) * glm::vec4(1.0, 0.0, 0.0, 1.0);
        camera_dir_y = glm::inverse(mView) * glm::vec4(0.0, 1.0, 0.0, 1.0);
        camera_dir_z = glm::inverse(mView) * glm::vec4(0.0, 0.0, -1.0, 1.0);
        // Apply a translation with inverted camera position
        mView = glm::translate(mView, (-cameraPos));
        
        glm::mat4 mPVM = mProjection * mView;

        glUseProgram(shader);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniformMatrix4fv(4, 1, GL_FALSE, &mPVM[0][0]);
        
        world.draw();
        //player.render(mPVM);
        entity.render(mPVM);

        // Render path
        Node* n = path->p;
        while (n) {
            if (!n->p)
                break;

            glm::mat4 mModel = glm::identity<glm::mat4>();
            mModel = glm::translate(mModel, glm::vec3(n->x, n->y, n->z));
            mModel = glm::scale(mModel, glm::vec3(0.5, 0.5, 0.5));
            mPVM = mProjection * mView * mModel;
            glUniformMatrix4fv(4, 1, GL_FALSE, &mPVM[0][0]);

            glBindVertexArray(cube.vao);
            glDrawArrays(GL_QUADS, 0, 32);

            mPVM = mProjection * mView;
            glUniformMatrix4fv(4, 1, GL_FALSE, &mPVM[0][0]);
            glm::vec4 p1 = glm::vec4(n->x, n->y, n->z, 1.0);
            glm::vec4 p2 = glm::vec4(n->p->x, n->p->y, n->p->z, 1.0);
            glBegin(GL_LINES);
            glVertex3f(p1.x+0.5, p1.y + 0.5, p1.z + 0.5);
            glVertex3f(p2.x + 0.5, p2.y + 0.5, p2.z + 0.5);
            glEnd();

            n = n->p;
        }

        //Draw startnode
        glm::mat4 mModel = glm::identity<glm::mat4>();
        mModel = glm::translate(mModel, glm::vec3(startNode.x, startNode.y, startNode.z));
        mModel = glm::scale(mModel, glm::vec3(0.5, 0.5, 0.5));
        mPVM = mProjection * mView * mModel;
        glUniformMatrix4fv(4, 1, GL_FALSE, &mPVM[0][0]);
        glBindVertexArray(startCube.vao);
        glDrawArrays(GL_QUADS, 0, 32);

        //Draw endnode
        mModel = glm::identity<glm::mat4>();
        mModel = glm::translate(mModel, glm::vec3(endNode.x, endNode.y, endNode.z));
        mModel = glm::scale(mModel, glm::vec3(0.5, 0.5, 0.5));
        mPVM = mProjection * mView * mModel;
        glUniformMatrix4fv(4, 1, GL_FALSE, &mPVM[0][0]);
        glBindVertexArray(endCube.vao);
        glDrawArrays(GL_QUADS, 0, 32);

        // Draw Crosshair
        glUseProgram(crossShader);
        glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
        glBegin(GL_QUADS);
        glVertex2f(-0.001,-0.001);
        glVertex2f(0.001, -0.001);
        glVertex2f(0.001, 0.001);
        glVertex2f(-0.001, 0.001);
        glEnd();
       
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}