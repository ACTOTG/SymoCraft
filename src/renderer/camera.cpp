//
// Created by Amo on 2022/6/18.
//

#include "renderer/camera.h"
#include "core.h"
#include "core/ECS/component.h"
#include "core/application.h"
#include "core/window.h"

namespace SymoCraft
{
    glm::mat4 Camera::GetCameraViewMat(ECS::Registry &registry) const
    {
        if (registry.HasComponent<Transform>(camera_id))
        {
            Transform& transform = registry.GetComponent<Transform>(camera_id);

            return glm::lookAt( transform.position
                    , transform.position + transform.front
                    , transform.up);
        }

        AmoLogger_Error("The %d camera does not have transform component", camera_id);
        return glm::mat4();
    }

    glm::mat4 Camera::GetCameraProjMat() const
    {
        return glm::perspective(
                glm::radians(fov),
                Application::GetWindow().GetAspectRatio(),
                0.1f,
                200.0f
                );
    }

    void Camera::SetFov(float new_fov)
    {
        fov = new_fov;
    }

    float Camera::GetFov() const
    {
        return fov;
    }
}