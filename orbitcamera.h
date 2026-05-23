#ifndef o_cam_h
#define o_cam_h

#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class OrbitCamera {
public:
    glm::vec3 Target;  // The target or "look-at" point
    float Distance;    // Distance from camera to target
    float Theta;       // Azimuthal angle
    float Phi;         // Polar angle

    OrbitCamera(glm::vec3 target, float distance)
        : Target(target), Distance(distance), Theta(0.0f), Phi(glm::radians(45.0f)) {}

    // Call this to update Theta and/or Phi based on user input or other logic
    void Orbit(float dTheta, float dPhi) {
        Theta += dTheta;
        Phi = glm::clamp(Phi + dPhi, 0.1f, glm::radians(179.9f));  // Avoids gimbal lock
    }

    // Calculate the camera's position from the target
    glm::vec3 GetPosition() const {
        float x = Distance * sin(Phi) * cos(Theta);
        float y = Distance * sin(Phi) * sin(Theta);
        float z = Distance * cos(Phi);
        return Target + glm::vec3(x, y, z);
    }

    // Assuming you're using glm, this will give you the view matrix for this camera
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(GetPosition(), Target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};


#endif