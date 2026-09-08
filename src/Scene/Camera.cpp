#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "ApplicationWindow.h"
#include "Input.h"
#include <iostream>

glm::vec3 Camera::s_WorldUp(0.0f, 1.0f, 0.0f);

Camera::Camera(GLFWwindow* window, float aspectRatio)
    : m_WindowPtr(window), m_AspectRatio(aspectRatio), m_CamPos(0.0f), m_CamFront(0.0f, 0.0f, -1.0f), m_CamUp(0.0f, 1.0f, 0.0f)
{

}

Camera::~Camera(){}

void Camera::Update(float playerX, float playerY, int levelWidth)
{
    // Mouse XY / Cam Yaw;Pitch
    glm::vec2 mousePos = Input::GetMousePos();
    double mouseX = mousePos.x, mouseY = mousePos.y;
    if (m_FirstCall)
    {
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
        m_FirstCall = 0;
    }
    double offsetX = mouseX - m_LastMouseX;
    double offsetY = m_LastMouseY - mouseY;
    m_LastMouseX = mouseX;
    m_LastMouseY = mouseY;

    if (!ApplicationWindow::Get().IsFocused() || ApplicationWindow::Get().IsPaused()) return;
    /*m_Yaw += offsetX * m_Sensitivity;
    m_Pitch += offsetY * m_Sensitivity;
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;*/

    glm::vec3 camDir(
        cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch)),
        sin(glm::radians(m_Pitch)),
        sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch))
    );
    m_CamFront = glm::normalize(camDir);
    m_CamRight = glm::normalize(glm::cross(m_CamFront, s_WorldUp));
    m_CamUp = glm::normalize(glm::cross(m_CamRight, m_CamFront));

    // Camera position (scroll X/Y)
    if (playerX >= m_ZoomLevelX/2 && m_ZoomLevelX + m_CamOffset.x <= levelWidth)
    {
        m_CamOffset.x = playerX-(m_ZoomLevelX/2);
    }
    else if (m_CamOffset.x > 0.f && playerX < (m_ZoomLevelX/2 + m_CamOffset.x))
    {
        m_CamOffset.x -= (m_ZoomLevelX / 2 + m_CamOffset.x) - playerX;
    }
    else if (m_CamOffset.x < 0) m_CamOffset.x = -0.0f;
    if (m_ZoomLevelX + m_CamOffset.x > levelWidth) m_CamOffset.x = levelWidth - m_ZoomLevelX;

    float zoomLevelY = m_ZoomLevelX / m_AspectRatio;
    if (playerY >= zoomLevelY / 2)
    {
        m_CamOffset.y = playerY - (zoomLevelY / 2);
    }
    else if (m_CamOffset.y > 0.f && playerY < (zoomLevelY / 2 + m_CamOffset.y))
    {
        m_CamOffset.y -= (zoomLevelY / 2 + m_CamOffset.y) - playerY;
    }
    else if (m_CamOffset.y < 0 && playerY <= zoomLevelY / 2) m_CamOffset.y = 0.0f;

    m_CamPos = glm::vec3(m_CamOffset.x-0.5, m_CamOffset.y, 1);

    m_ViewMatrix = glm::lookAt(m_CamPos, m_CamPos + m_CamFront, m_CamUp);

    // Zoom
    if (Input::IsKeyPressed(GLFW_KEY_HOME))
    {
        m_ZoomLevelX -= 0.1f;
    }
    if (Input::IsKeyPressed(GLFW_KEY_END))
    {
        m_ZoomLevelX += 0.1f;
    }
    if (m_ZoomLevelX < 1.0f) m_ZoomLevelX = 1.0f;
    if (m_ZoomLevelX > levelWidth) m_ZoomLevelX = levelWidth;

    int halfWidth = ApplicationWindow::Get().GetWidth()/2;
    int halfHeight= ApplicationWindow::Get().GetHeight()/2;
    m_ProjMatrix = glm::ortho(0.f, m_ZoomLevelX, 0.f, zoomLevelY, m_Near, m_Far);
}