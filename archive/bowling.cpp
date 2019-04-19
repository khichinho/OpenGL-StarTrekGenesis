#include <iostream>
#include <math.h>

#include <glad/glad.h>
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include <SOIL/SOIL.h>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0, 2.5, 8.0));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader pinShader("pin.vs", "pin.fs");

    // load models
    vector<Model> pinModels;
    for(int i=0; i<10; i++){
        Model pinModel("../resources/Bowling/BowlingPins/BowlingPins.obj");
        pinModels.push_back(pinModel);
    }

    // build and compile shaders
    Shader ballShader("ball.vs", "ball.fs");

    // load models
    Model ballModel("../resources/Bowling/Ball/Ball.obj");










    // build and compile our shader zprogram
    Shader floorShader("floor_camera.vs", "floor_camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions          // texture coords
         100.0f, 0.0f,  0.0f,   1.0f, 1.0f, // top right
         100.0f, 0.0f, -1500.0f,   1.0f, 0.0f, // bottom right
        -100.0f, 0.0f, -1500.0f,   0.0f, 0.0f, // bottom left
        -100.0f, 0.0f,  0.0f,   0.0f, 1.0f,  // top left

         100.0f, 30.0f,  0.0f,   0.0f, 1.0f, // gutter back right
         100.0f, 30.0f, -1500.0f,   0.0f, 0.0f, // gutter front right
        -100.0f, 30.0f, -1500.0f,   1.0f, 0.0f, // gutter front left
        -100.0f, 30.0f,  0.0f,   1.0f, 1.0f  // gutter back left
    };
    unsigned int indices[] = {  
        0, 1, 3, // base
        1, 2, 3, // base
        0, 1, 5, // gutter right
        5, 0, 4, // gutter right
        7, 6, 2, //gutter left
        2, 7, 3 //gutter left
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height;
    unsigned char *data = SOIL_load_image("wooden_floor.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    SOIL_free_image_data(data);

    // activate shader
    floorShader.use();
    floorShader.setInt("texture1", 0);


    // ? draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);







    int once = 0;
    int once1 = 0;

    int followBall = 1;

    int isBallonFloor = 1;
    float ballWeight = 10.0;
    float pinWeight = 5.0;

    float ballHitAt = -1360.0;
    float ballSpin = -60.0;
    float pinsDistance = 20.0f;

    glm::vec3 ballSpeed = glm::vec3(0, 0, -10.0);
    glm::vec3 omega = glm::vec3(ballSpeed.x, ballSpeed.y, ballSpeed.z);
    glm::vec3 angle = glm::vec3(0, 0, 0);

    float initialX = 0.0f;

    glm::vec3 pinLocation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 ballLocation = glm::vec3(initialX, 0.0f, 0.0f);

    vector<glm::vec3> pinLocationVector;
    vector<glm::vec3> pinHitDirection;
    vector<int> isPinHit;
    vector<int> isPinHitComplete;
    vector<float> pinFallAngle;
    vector<float> pinFallSpeed;

    for(int i=0; i<10; i++){
        isPinHit.push_back(0);
        isPinHitComplete.push_back(0);
        pinHitDirection.push_back(glm::vec3(0,0,0));
        pinFallAngle.push_back(0.0);
        pinFallSpeed.push_back(0.0);
    }





    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





        ///////////// PINS ////////////////
        pinShader.use();

        // view/projection transformations
        glm::mat4 pinProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        pinShader.setMat4("projection", pinProjection);

        glm::mat4 pinView = camera.GetViewMatrix();
        pinShader.setMat4("view", pinView);
        
        vector<glm::mat4> pinModel;

        for(int i=0; i<10; i++)
        {
            pinModel.push_back(glm::mat4(1.0f));
            pinShader.setMat4("model", pinModel[i]);
            pinModel[i] = glm::scale(pinModel[i], glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down

            // render the loaded model
            if(i==0){
                pinLocation += glm::vec3(1.5*pinsDistance, 0.0f, -1450.0f);
            }
            pinModel[i] = glm::translate(pinModel[i], pinLocation);

            pinLocationVector.push_back(pinLocation + glm::vec3(0, 0.0f, 0));

            if(isPinHit.at(i) == 1){
                pinModel[i] = glm::translate(pinModel[i], glm::vec3(0, 6.0f, 0));
                if(pinFallAngle.at(i) > -89.9)
                {
                    pinModel[i] = glm::translate(pinModel[i], glm::vec3(pinHitDirection.at(i).x, 0, pinHitDirection.at(i).z));
                    pinLocationVector.at(i).x += pinHitDirection.at(i).x;
                    pinLocationVector.at(i).z += pinHitDirection.at(i).z;
                    if((pinFallAngle.at(i) + -1.0*pinFallSpeed.at(i)) < -90.0){
                        pinFallAngle.at(i) = -90.0f;
                        isPinHitComplete.at(i) = 1;
                        cout << "PIN " << i << " FALL COMPLETE" << endl;
                    }
                    else
                    {
                        pinFallAngle.at(i) += -1.0*pinFallSpeed.at(i);
                        if(pinFallAngle.at(i) < -89.9)
                        {
                            isPinHitComplete.at(i) = 1;
                        }
                    }
                }
                pinModel[i] = glm::rotate(pinModel[i], glm::radians(pinFallAngle.at(i)), glm::vec3( pinHitDirection.at(i).z, 0, -pinHitDirection.at(i).x));
            }
            pinShader.setMat4("model", pinModel[i]);
            pinModels[i].Draw(pinShader);

            // arrangment for pin structure
            if(i==3){
                pinLocation += glm::vec3(2.5*pinsDistance, 0.0f, pinsDistance);
            }
            else if(i==6){
                pinLocation += glm::vec3(1.5*pinsDistance, 0.0f, pinsDistance);
            }
            else if(i==8){
                pinLocation += glm::vec3(0.5*pinsDistance, 0.0f, pinsDistance);
            }
            else{
                pinLocation += glm::vec3(-pinsDistance, 0.0f, 0.0f);
            }
        }
        pinLocation = glm::vec3(0.0f, 0.0f, 0.0f);







        ///////////// BALL ////////////////
        // view/projection transformations
        glm::mat4 ballProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        pinShader.setMat4("projection", ballProjection);

        glm::mat4 ballView = camera.GetViewMatrix();
        pinShader.setMat4("view", ballView);
        
        glm::mat4 ball_Model = glm::mat4(1.0f);
        pinShader.setMat4("model", ball_Model);

        ball_Model = glm::scale(ball_Model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down

        angle += omega;
        ballLocation += ballSpeed;

        ball_Model = glm::translate(ball_Model, ballLocation);
        ball_Model = glm::translate(ball_Model, glm::vec3(0, 11.0f, 0));
        // ball_Model = glm::translate(ball_Model, glm::vec3(0, - 11.0f*(cos(M_PI*angle.z/180)), 0));

        // ball_Model = glm::rotate(ball_Model, glm::radians(angle.x), glm::vec3(0, ballLocation.z, 0));
        // ball_Model = glm::rotate(ball_Model, glm::radians(angle.y), glm::vec3(0, 0, ballLocation.z));
        // ball_Model = glm::rotate(ball_Model, glm::radians(angle.z), glm::vec3(ballLocation.z, 0, 0));

        ball_Model = glm::rotate(ball_Model, glm::radians(angle.z), glm::vec3(0, 0, 1.0));


        // don't forget to enable shader before setting uniforms
        ballShader.use();

        // view/projection transformations
        ballShader.setMat4("projection", ballProjection);
        ballShader.setMat4("view", ballView);

        // render the loaded model
        ballShader.setMat4("model", ball_Model);

        if(isBallonFloor == 1 ){ ballModel.Draw(ballShader); }




        // intersection loop here
        for(int i=0; i<10; i++){
            if(once == 0){
                cout << "PIN " << i << " x: " << pinLocationVector[i].x << " y: " << pinLocationVector[i].y << " z: " << pinLocationVector[i].z << endl;
            }
            if(glm::distance(ballLocation, pinLocationVector[i]) <= ((pinsDistance/2) + 2) && isPinHit.at(i) == 0){
                isPinHit.at(i) = 1;
                pinHitDirection.at(i) = glm::normalize(ballLocation - pinLocationVector[i]);
                pinFallSpeed.at(i) = glm::length(ballLocation - pinLocationVector[i]);
                cout << "BALL HITS PIN AT x: " << ballLocation.x << " y: " << ballLocation.y << " z: " << ballLocation.z << endl;
                if(pinWeight/ballWeight >= 2){
                    ballSpeed = -ballSpeed;
                    omega = ballSpeed;
                }
                else{
                    ballSpeed -= pinWeight*glm::vec3(pinHitDirection.at(i).x*ballSpeed.x, pinHitDirection.at(i).y*ballSpeed.y, pinHitDirection.at(i).z*ballSpeed.z)/ballWeight;
                    omega = ballSpeed;
                }
            }
        }
        
        for(int i=0; i<10; i++)
        {
            if(isPinHitComplete.at(i) == 1)
            {
                for(int j=0; j<10; j++)
                {
                    if(isPinHit.at(j) == 0 && i!=j){
                        // distance between is 42.4
                        if(glm::distance(pinLocationVector[i], pinLocationVector[j]) <= (pinsDistance + 1) && isPinHit.at(j) == 0){
                            // cout<< glm::distance(pinLocationVector[i], pinLocationVector[j])<<endl;
                            isPinHit.at(j) = 1;
                            pinHitDirection.at(j) = glm::normalize(pinLocationVector[i] - pinLocationVector[j]);
                            pinFallSpeed.at(j) = glm::length(pinFallSpeed.at(i) - pinLocationVector[i]);
                            cout << "PIN HITS PIN AT x: " << pinLocationVector[j].x << " y: " << pinLocationVector[j].y << " z: " << pinLocationVector[j].z << endl;
                        }
                    }
                }
            }
        }

        once += 1;
        if(ballLocation.z < -1500 ){
            isBallonFloor = 0;
            if(once1 == 0){
                for(int i=0; i<10; i++){
                    cout << "PIN " << i << " x: " << pinLocationVector[i].x << " y: " << pinLocationVector[i].y << " z: " << pinLocationVector[i].z << endl;
                }
                once1 = 1;
            }
        }

        ballLocation.x = initialX + ballSpin*sin(M_PI*ballLocation.z/ballHitAt);
        if(ballLocation.x > 95 || ballLocation.x < -95 || ballLocation.z < -1500 || ballLocation.z > 10 ){
            isBallonFloor = 0;
            ballSpeed = glm::vec3(0,0,0);
        }
        // camera movement
        if(followBall == 1 && ballLocation.z > -1350){
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }




        // view/projection transformations
        glm::mat4 floorProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        pinShader.setMat4("projection", floorProjection);

        glm::mat4 floorView = camera.GetViewMatrix();
        pinShader.setMat4("view", floorView);
        
        glm::mat4 floorModel = glm::mat4(1.0f);
        pinShader.setMat4("model", floorModel);

        floorModel = glm::scale(floorModel, glm::vec3(0.02f, 0.02f, 0.02f)); // it's a bit too big for our scene, so scale it down

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        floorShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        floorShader.setMat4("projection", floorProjection);

        // camera/view transformation
        floorShader.setMat4("view", floorView);

        // render boxes
        glBindVertexArray(VAO);
        floorShader.setMat4("model", floorModel);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);





        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, 5*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, 5*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, 5*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, 5*deltaTime);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}