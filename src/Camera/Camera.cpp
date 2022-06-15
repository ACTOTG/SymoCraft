//
// Created by Amo on 2022/6/12.
//

#include "Camera/camera.h"
#include "core.h"
#include "Core/application.h"
#include "Core/window.h"

namespace SymoCraft
{

    const glm::vec3 Camera::world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera::Camera( float width, float height, glm::vec3 c_pos)
            : camera_pos(c_pos), last_x(width), last_y(height)
    {
        yaw = -90.0f;   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results
        // in a direction vector pointing to the right,
        // so we initially rotate a bit to the left.
        pitch = 0.0f;
        fov = 45.0f;
        UpdateCameraVectors();
    }

    void Camera::InsMouseMovementCallBack(GLFWwindow *window, double xpos_in, double ypos_in)
    {
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

        const float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // calculate the angles of camera
        yaw += xoffset;
        pitch += yoffset;

        // add some constraints to the camera avoiding the gimbal lock
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }

    void Camera::InsMouseScrollCallBack(GLFWwindow *window, double x_offset, double y_offset)
    {
        fov -= (float)y_offset;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;
    }

    void Camera::UpdateCameraVectors()
    {
        // calculate camera front
        glm::vec3 front_direction;
        front_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_direction.y = sin(glm::radians(pitch));

        camera_front = glm::normalize(front_direction);
        // calculate camera right
        camera_right = glm::normalize(glm::cross(camera_front, world_up));
        camera_up = glm::normalize(glm::cross(camera_right, camera_front));
    }

    void Camera::CameraMoveBy(MoveDirection direction, float displacement)
    {
        switch (direction) {
            case FORWARD:
                camera_pos += camera_front * displacement;
                break;
            case BACKWARD:
                camera_pos -= camera_front * displacement;
                break;
            case LEFT:
                camera_pos -= camera_right * displacement;
                break;
            case RIGHT:
                camera_pos += camera_right * displacement;
                break;
            case UPWARD:
                camera_pos += world_up * displacement;
                break;
            case DOWNWARD:
                camera_pos -= world_up * displacement;
                break;
            default:
                break;
        }
    }

    glm::mat4 Camera::GetCameraViewMat() const
    {
        return glm::lookAt( camera_pos
                ,camera_pos + camera_front
                ,camera_up);;
    }

    glm::mat4 Camera::GetCameraProjMat() const
    {
        Window& window = Application::GetWindow();
        return glm::perspective( glm::radians( fov), window.GetAspectRatio() , 0.1f, 100.0f);
    }
}