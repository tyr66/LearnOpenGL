#include <cmath>
#include <algorithm>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Input.h"

Camera::Camera(): _pos(0.0f, 0.0f, 3.0f), _front(0.0f, 0.0f, -1.0f), _up(0.0f, 1.0f, 0.0f), _fov(45.0f), _near(0.1f), _far(100.0f), _sensitivity(0.1f), _moveSpeed(2.5f), _scaleSpeed(0.02f)
{
    _pitch = 0.0f;
    _yaw = 180.0f;
}

void Camera::SetCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
}

void Camera::SetFrustum(float fov, float near, float far)
{
}

void Camera::UpdateCameraFront(double mouseX, double mouseY)
{
    // printf("xoffset = %f, yoffset = %f\n", mouseX - _lastX, mouseY - _lastY);

    if (_isFirstUpdate) {
        _isFirstUpdate = false;
        _lastX = mouseX;
        _lastY = mouseY;
    }

    float xoffset = mouseX  - _lastX;
    float yoffset = mouseY - _lastY;

    _pitch -= yoffset * _sensitivity;
    _yaw -= xoffset * _sensitivity;

    _pitch = std::clamp(_pitch, -89.0f, 89.0f);

    float x = cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));
    float y = sin(glm::radians(_pitch));
    float z = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));

    _front = glm::vec3(x, y, z);
    _front = glm::normalize(_front);

    _lastX = mouseX;
    _lastY = mouseY;

    // printf("front is {%f, %f, %f}\n", _front.x, _front.y, _front.z);
}


void Camera::UpdateCameraPos()
{
    float delta = glfwGetTime() - _lastTime;
    float movedelta = _moveSpeed * delta;

    if (Input::IsKeyPress(GLFW_KEY_W)) {
        _pos += _front * movedelta;
    }
    if (Input::IsKeyPress(GLFW_KEY_S)) {
        _pos += -_front * movedelta;
    }
    if (Input::IsKeyPress(GLFW_KEY_A)) {
        _pos += -glm::normalize(glm::cross(_front, _up)) * movedelta;
    }
    if (Input::IsKeyPress(GLFW_KEY_D)) {
        _pos += glm::normalize(glm::cross(_front, _up) )* movedelta;
    }

    _lastTime = glfwGetTime();

    // printf("pos is {%f, %f, %f}\n", _pos.x, _pos.y, _pos.z);
}

void Camera::UpdateCameraFov(double xOffset, double yOffset)
{
    // printf("xscrolloffset = %f, yscrollOffset = %f\n", xOffset, yOffset);

    if (_fov >= 1.0f && _fov <= 45.0f) {
        _fov -= yOffset * _scaleSpeed;
    }else if (_fov > 45.0f) {
        _fov = 45.0f;
    } else {
        _fov = 1.0f;
    }
}

glm::mat4 Camera::GetProjMat4(float w, float h)
{
    return glm::perspective(_fov, w / h, _near, _far);
}

void Camera::PrintfInfo()
{
    printf("camera pos : {%f, %f, %f}, front : {%f, %f, %f}, up : {%f, %f, %f}\n", _pos.x, _pos.y, _pos.z, _front.x, _front.y, _front.z, _up.x, _up.y, _up.z);
}

glm::mat4 Camera::GetViewMat4()
{
    return glm::lookAt(_pos, _pos + _front, _up);
}
