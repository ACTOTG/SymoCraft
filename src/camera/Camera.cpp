//
// Created by Amo on 2022/6/12.
//

#include "camera/camera.h"
#include "core.h"
#include "core/application.h"
#include "core/window.h"
#include "core/ECS/registry.h"
#include "core/ECS/component.h"

namespace SymoCraft
{

    const glm::vec3 Camera::world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera::Camera( float width, float height, glm::vec3 c_pos)
            : last_x(width), last_y(height)
    {
        ECS::Registry &registry = Application::GetRegistry();
        entity_id = registry.CreateEntity();
        registry.AddComponent<Transform>(entity_id);
        Transform &transform = registry.GetComponent<Transform>(entity_id);
        transform.yaw = -90.0f;   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results
        // in a direction vector pointing to the right,
        // so we initially rotate a bit to the left.
        transform.pitch = 0.0f;
        transform.position = c_pos;
        fov = 45.0f;
    }

    void Camera::InsMouseMovementCallBack(GLFWwindow *window, double xpos_in, double ypos_in)
    {
        ECS::Registry &registry = Application::GetRegistry();
        Transform &transform = registry.GetComponent<Transform>(entity_id);

        auto xpos = static_cast<float>(xpos_in);
        auto ypos = static_cast<float>(ypos_in);

        // modify the first enter of the mouse
        if (first_enter)
        {
            last_x = xpos;
            last_y = ypos;
            first_enter = false;
        }

        float xoffset = xpos - last_x;
        float yoffset = last_y - ypos;   // reversed since y-coordinates range from bottom to top
        last_x = xpos;
        last_y = ypos;

        const float sensitivity = 0.001f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // calculate the angles of camera
        transform.yaw += xoffset;
        transform.pitch += yoffset;

        // add some constraints to the camera avoiding the gimbal lock
        if (transform.pitch > 89.0f)
            transform.pitch = 89.0f;
        if (transform.pitch < -89.0f)
            transform.pitch = -89.0f;

        // update Front, Right and Up Vectors using the updated Euler angles
    }

    void Camera::InsMouseScrollCallBack(GLFWwindow *window, double x_offset, double y_offset)
    {
        fov -= (float)y_offset;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;
    }

    void Camera::CameraMoveBy(MoveDirection direction, float displacement)
    {
        ECS::Registry &registry = Application::GetRegistry();
        Transform &transform = registry.GetComponent<Transform>(entity_id);
        switch (direction) {
            case FORWARD:
                transform.position += transform.front * displacement;
                break;
            case BACKWARD:
                transform.position -= transform.front * displacement;
                break;
            case LEFT:
                transform.position -= transform.right * displacement;
                break;
            case RIGHT:
                transform.position += transform.right * displacement;
                break;
            case UPWARD:
                transform.position += world_up * displacement;
                break;
            case DOWNWARD:
                transform.position -= world_up * displacement;
                break;
            default:
                break;
        }
    }

    glm::mat4 Camera::GetCameraViewMat() const
    {
        ECS::Registry &registry = Application::GetRegistry();
        Transform &transform = registry.GetComponent<Transform>(entity_id);
        return glm::lookAt( transform.position
                ,transform.position + transform.front
                ,transform.up);;
    }

    glm::mat4 Camera::GetCameraProjMat() const
    {
        Window& window = Application::GetWindow();
        return glm::perspective( glm::radians( fov), window.GetAspectRatio() , 0.1f, 100.0f);
    }


    glm::vec3 Camera::GetCameraPos() const
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        return transform.position;
    }

    glm::vec2 Camera::GetCameraPos_vec2() const
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        return glm::vec2(transform.position.x, transform.position.z);
    }

    void Camera::SetCameraPos(glm::vec3& c_pos)
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        transform.position = c_pos;
    }

    float Camera::GetYaw() const
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        return transform.yaw;
    }

    void Camera::SetYaw(float &_yaw)
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        transform.yaw = _yaw;
    }

    float Camera::GetPitch() const
    {

        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        return transform.pitch;
    }

    void Camera::SetPitch(float &_pitch)
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        transform.pitch = _pitch;
    }

    float Camera::GetFov() const
    {
        return fov;
    }

    glm::vec3 Camera::GetCameraFront() const
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        return transform.front;
    }

    glm::vec3 Camera::GetCameraUp() const
    {
        ECS::Registry &registry = Application::GetRegistry();
        auto &transform = registry.GetComponent<Transform>(entity_id);
        return transform.up;
    }

}