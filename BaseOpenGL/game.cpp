#include "game.h"
#include "resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/compatibility.hpp>

#include "camera.h"
#include "plane.h"
#include "game_object.h"
#include <queue>
#include <random>
#include "text_renderer.h"
#include <iomanip>

#include <stdlib.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

string dir = "C:/Users/Aaron/source/repos/Plane Simulator/BaseOpenGL/";

unsigned int cubemapTexture;

Camera camera(glm::vec3(-10.0f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0, -20);
glm::vec3 cameraOffset;
glm::vec3 firstPersonView = glm::vec3(0.85f, 0.5f, 0.0f);
bool firstPerson = false;
GameObject* PlaneGO;
GameObject* Propeller;
bool useQuat = false;

Plane plane(glm::vec3(0.f, 10.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f), 0, 0);


string Current_Text;
bool menu = false;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), LightPos(1.2f, 500.0f, 2.0f)
{

}

unsigned int skyboxVAO, skyboxVBO, gridVAO, gridVBO;

Game::~Game()
{
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
}

void Game::Init()
{
#pragma region SKYBOX_SETUP
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float gridVertices[] = {
        // positions          
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };

    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	
	// grid VAO
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), &gridVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        dir + "resources/skybox/right.jpg",
            dir + "resources/skybox/left.jpg",
            dir + "resources/skybox/top.jpg",
            dir + "resources/skybox/bottom.jpg",
            dir + "resources/skybox/front.jpg",
            dir + "resources/skybox/back.jpg"
    };
    cubemapTexture = loadCubemap(faces);
#pragma endregion SKYBOX_SETUP

    // build and compile shaders
    // -------------------------
    ResourceManager::LoadShader("cubemap.vert", "cubemap.frag", nullptr, "Cubemap");
    ResourceManager::GetShader("Cubemap").SetInteger("skybox", 0);
    ResourceManager::LoadShader("skybox.vert", "skybox.frag", nullptr, "Skybox");
    ResourceManager::GetShader("Skybox").SetInteger("skybox", 0);
    ResourceManager::LoadShader("grid.vert", "grid.frag", nullptr, "Grid");
    ResourceManager::LoadShader("blinnTexture.vert", "blinnTexture.frag", nullptr, "Blinn");
    //Models
    PlaneGO = new GameObject(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.02f, 0.02f, 0.02f),
        Model("plane/plane.obj"), glm::vec3(1.0f, 1.0f, 1.0f));
    Propeller = new GameObject(glm::vec3(-117.5f, 13.0f, 0.0f), glm::vec3(0.02f, 0.02f, 0.02f),
        Model("plane/p3.obj"), glm::vec3(1.0f, 1.0f, 1.0f));
    PlaneGO->AddChild(Propeller);
    
    /*Propeller = new GameObject(glm::vec3(20.0f, 0.0f, 0.0f), glm::vec3(5.0f, 5.0f, 5.0f),
        Model("sphere.obj"), glm::vec3(0.0f, 0.3f, 0.7f));*/

    cameraOffset = camera.Position - PlaneGO->Position;

}

float d_etaR = 0.65f;
float d_etaG = 0.66f;
float d_etaB = 0.67f;
float d_fresnelpower = 5.0f;

float s_etaR = 1.0f;
float s_etaG = 0.967f;
float s_etaB = 0.919f;
float s_fresnelpower = 0.0f;

float g_etaR = 0.65f;
float g_etaG = 0.66f;
float g_etaB = 0.67f;
float g_fresnelpower = 0.0f;

float h_etaR = 0.65f;
float h_etaG = 0.66f;
float h_etaB = 0.67f;
float h_fresnelpower = 5.0f;

float propSpin = 0;
void Game::Update(float dt)
{
    plane.MoveForward(dt);
    PlaneGO->Position = plane.Position;
    if (!useQuat) {
        PlaneGO->Yaw = plane.Yaw;
        PlaneGO->Pitch = plane.Pitch;
        PlaneGO->Roll = plane.Roll;
    }
    else
    {
        PlaneGO->Quaternion = plane.Quaternion;
    }

    if (!firstPerson) {
        camera.SetPos(PlaneGO->Position + cameraOffset);
        camera.SetYawPitchRoll(0, -20, 0);
    }
    else {
        //camera.SetPos(plane.Position);
        glm::mat4 planeMat = glm::mat4(1.0f);
        planeMat = glm::translate(planeMat, plane.Position);
        planeMat *= glm::yawPitchRoll(glm::radians(plane.Yaw), glm::radians(plane.Pitch), glm::radians(plane.Roll));
        glm::mat4 cameraMat = glm::mat4(1.0f);
        cameraMat = glm::translate(cameraMat, firstPersonView);
        camera.SetYawPitchRoll(-plane.Yaw, plane.Roll, plane.Pitch);
        //cameraMat = glm::rotate(cameraMat, glm::radians(-plane.Yaw), glm::vec3(0, 1, 0));
        //cameraMat = glm::rotate(cameraMat, glm::radians(plane.Roll), glm::vec3(1, 0, 0));
        //cameraMat = glm::rotate(cameraMat, glm::radians(plane.Pitch), glm::vec3(0, 0, 1));
        camera.SetPosRotMatrix(planeMat * cameraMat);
        //model_child = glm::scale(model_child, Size*Size);
        
    	//camera.SetYawPitchRoll(-plane.Yaw, plane.Roll, plane.Pitch);
        /*camera.SetPos(plane.Position + firstPersonView);*/
    }

    propSpin += 10;
    PlaneGO->UpdateChildRot(0, glm::vec3(propSpin, 0, 0));
    if (propSpin == 360)
        propSpin = 0;
    {
        ImGui::Begin("Settings");
        float lightPos[3] = { LightPos.x, LightPos.y, LightPos.z };

        if (ImGui::CollapsingHeader("Euler"))
        {
            if (ImGui::Button("Gimbol Lock"))
            {
                plane.Pitch = 90;
                plane.Yaw = 90;
                plane.Roll = 90;
            }
        }
        if (ImGui::CollapsingHeader("View"))
        {
            ImGui::Checkbox("First Person", &firstPerson);
        }
        if (ImGui::CollapsingHeader("Quaternion"))
        {
            ImGui::Checkbox("Enable Quaternion", &useQuat);
            plane.useQuat = useQuat;
        }
        if (ImGui::CollapsingHeader("Light"))
        {
            ImGui::SliderFloat3("Position", lightPos, -500.0f, 500.0f);
            LightPos.x = lightPos[0];
            LightPos.y = lightPos[1];
            LightPos.z = lightPos[2];
        }
        

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
        ImGui::End();
    }

}

bool keyDown = false;
int rotationSpeed = 40;
void Game::ProcessInput(float dt, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        plane.ProcessKeyboard(2, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        plane.ProcessKeyboard(3, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        plane.ProcessKeyboard(1, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        plane.ProcessKeyboard(0, dt);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        plane.ProcessKeyboard(4, dt);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        plane.ProcessKeyboard(5, dt);


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keyDown)
    {
        if(!menu)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        keyDown = true;
        menu = !menu;
    }
    else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && keyDown)
    {
        keyDown = false;
    }
}


void Game::Render()
{
    ResourceManager::GetShader("Blinn").Use();
    Shader shader = ResourceManager::GetShader("Blinn");
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)this->Width / this->Height, 0.1f, 100.0f);
    shader.SetMatrix4("model", model);
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
    shader.SetVector3f("viewPos", camera.Position);
    shader.SetVector3f("light.position", this->LightPos);
    shader.SetFloat("specSize", 1.0f);
    shader.SetFloat("specStrength", 128);
    shader.SetFloat("ambient", 0.02f);

    PlaneGO->DrawPlane(shader, useQuat);
    
    ResourceManager::GetShader("Grid").Use();
    shader = ResourceManager::GetShader("Grid");
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("proj", projection);
    shader.SetVector3f("pos", camera.Position);
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    ResourceManager::GetShader("Skybox").Use();
    shader = ResourceManager::GetShader("Skybox");
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    shader.SetMatrix4("view", view);
    shader.SetMatrix4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::ProcessMouseMovement(float xoffset, float yoffset)
{
    if(!menu)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void Game::ProcessMouseScroll(float yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int Game::loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}