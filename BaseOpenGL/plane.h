#ifndef PLANE_H
#define PLANE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

// Default camera values
const float pYAW = -90.0f;
const float pPITCH = 0.0f;
const float pROLL = 0.0f;
const float pSPEED = 20.0f;
const float pSENSITIVITY = 0.1f;
const float pZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Plane
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool useQuat = false;
    glm::quat Quaternion = glm::quat (0, 0, 0, 1);

    // constructor with vectors
    Plane(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = pYAW, float pitch = pPITCH, float roll = pROLL) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(pSPEED), MouseSensitivity(pSENSITIVITY), Zoom(pZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Roll = roll;
        updatePlaneVectors();
    }
    // constructor with scalar values
    Plane(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(pSPEED), MouseSensitivity(pSENSITIVITY), Zoom(pZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Roll = roll;
        updatePlaneVectors();
    }

    void SetPos(glm::vec3 position)
    {
        Position = position;
    }

    void SetPitch(float pitch)
    {
        Pitch = pitch;
        updatePlaneVectors();
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(int direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        if (!useQuat)
        {
            if (direction == 0)
                Pitch += MovementSpeed * deltaTime;
            else if (direction == 1)
                Pitch -= MovementSpeed * deltaTime;
            if (direction == 2)
                Roll -= MovementSpeed * deltaTime;
            else if (direction == 3)
                Roll += MovementSpeed * deltaTime;
            if (direction == 4)
                Yaw -= MovementSpeed * deltaTime;
            else if (direction == 5)
                Yaw += MovementSpeed * deltaTime;
        }
        else
        {
            glm::quat qYaw, qPitch, qRoll;
            
            if (direction == 4) {
                qYaw = glm::angleAxis(glm::radians(-MovementSpeed * deltaTime), Up);
                Quaternion *= qYaw;
            }
            else if (direction == 5) {
                qYaw = glm::angleAxis(glm::radians(MovementSpeed * deltaTime), Up);
                Quaternion *= qYaw;
            }
            if (direction == 0) {
                qPitch = glm::angleAxis(glm::radians(MovementSpeed * deltaTime), Right);
                Quaternion *= qPitch;
            }
            else if (direction == 1) {
                qPitch = glm::angleAxis(glm::radians(-MovementSpeed * deltaTime), Right);
                Quaternion *= qPitch;
            }
            if (direction == 2) {
                qRoll = glm::angleAxis(glm::radians(-MovementSpeed * deltaTime), Front);
                Quaternion *= qRoll;
            }
            else if (direction == 3) {
                qRoll = glm::angleAxis(glm::radians(MovementSpeed * deltaTime), Front);
                Quaternion *= qRoll;
            }

            //glm::quat rot = qYaw * qPitch * qRoll;
            //Quaternion *= rot;

        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updatePlaneVectors();
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updatePlaneVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void MoveForward(float deltaTime)
    {
        glm::vec3 f;
        if (!useQuat) {
            glm::mat4 forward = glm::yawPitchRoll(glm::radians(Yaw), glm::radians(Pitch), glm::radians(Roll));
            f = normalize(forward[0]);
        }
        else
        {
            glm::mat4 qforward = glm::toMat4(Quaternion);
            f = normalize(qforward[0]);
        }
        float velocity = (MovementSpeed/2) * deltaTime;
        Position += f * velocity;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updatePlaneVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        if (!useQuat) {
            front.y = sin(glm::radians(Pitch));
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
            // also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up = glm::normalize(glm::cross(Right, Front));
            Up = glm::mat3(roll_mat) * Up;
        }
        else
        {
            front.x = 2 * (Quaternion.x * Quaternion.z + Quaternion.w * Quaternion.y);
            front.y = 2 * (Quaternion.y * Quaternion.z - Quaternion.w * Quaternion.x);
            front.z = 1 - 2 * (Quaternion.x * Quaternion.x + Quaternion.y * Quaternion.y);
            Front = glm::normalize(front);
            glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
            glm::vec3 right;
            right.x = 1 - 2 * (Quaternion.y * Quaternion.y + Quaternion.z * Quaternion.z);
            right.y = 2 * (Quaternion.x * Quaternion.y + Quaternion.w * Quaternion.z);
            right.z = 2 * (Quaternion.x * Quaternion.z - Quaternion.w * Quaternion.y);
            Right = glm::normalize(right);
            glm::vec3 up;
            up.x = 2 * (Quaternion.x * Quaternion.y - Quaternion.w * Quaternion.z);
            up.y = 1 - 2 * (Quaternion.x * Quaternion.x + Quaternion.z * Quaternion.z);
            up.z = 2 * (Quaternion.y * Quaternion.z + Quaternion.w * Quaternion.x);
            Up = glm::normalize(up);
            Up = glm::mat3(roll_mat) * Up;

        }
        
    }
};
#endif