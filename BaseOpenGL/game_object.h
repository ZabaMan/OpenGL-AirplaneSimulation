#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "model.h"

#include <algorithm>
#include <iostream>
#include <list>

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec3   Position, Size, Rotation;
    float Yaw, Pitch, Roll;
    glm::quat Quaternion;
    glm::mat4 TransformMatrix;
    glm::vec3   Color;
    bool        HasParent;
    bool        Destroyed;
    GameObject* Parent;
    struct Child
    {
        GameObject* ChildGO;
        glm::vec3 posOffset;
        glm::vec3 rotOffset;

        Child(GameObject* GO, glm::vec3 pos, glm::vec3 rot);

        void UpdatePosOffset(glm::vec3 pos);
    };
    std::vector<Child> Children;
    // render state
    Model   model;
    // constructor(s)
    GameObject();
    GameObject(glm::vec3 pos, glm::vec3 size, Model model, glm::vec3 color = glm::vec3(1.0f));
    // draw sprite
    virtual void Draw(Shader& shader);
    void DrawPlane(Shader& shader, bool useQuat);
    void AddChild(GameObject* GO);
    void UpdateChildPos(int c, glm::vec2 pos);
    void UpdateChildRot(int c, glm::vec3 rot);
    void CheckChildPos(int c);
    glm::vec3 GetChildPos(int c);
    void DrawChild(glm::mat4 model_, Shader& shader);
};

#endif