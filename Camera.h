#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    void SetCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);
    void SetFrustum(float fov, float near, float far);
    void UpdateCameraFront(double mouseX, double mouseY);
    void UpdateCameraPos();
    void UpdateCameraFov(double xOffset, double yOffset);
    glm::mat4 GetViewMat4();
    glm::mat4 GetProjMat4(float w, float h);
private:
    float _near;
    float _far;
    float _yaw;
    float _pitch;
    float _lastX;
    float _lastY;
    float _lastTime;
    float _sensitivity;
    float _moveSpeed;
    float _scaleSpeed;
    float _fov;
    glm::vec3 _pos;
    glm::vec3 _front;
    glm::vec3 _up;
    bool _isFirstUpdate{true};
};
