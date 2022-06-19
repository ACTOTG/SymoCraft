//
// Created by Amo on 2022/6/18.
//

#ifndef SYMOCRAFT_CAMERA_H
#define SYMOCRAFT_CAMERA_H
#include "core.h"
#include "core/ECS/registry.h"
namespace SymoCraft
{
    class Camera
    {
    public:
        // Return View Matrix
        glm::mat4 GetCameraViewMat(ECS::Registry &registry) const;

        // Return Projection Matrix
        glm::mat4 GetCameraProjMat() const;

        // Set Fov
        // Parameters: new fov
        void SetFov( float new_fov);

        // return fov
        float GetFov() const;

    private:
        ECS::EntityId camera_id;
        float fov;
    };
}

#endif //SYMOCRAFT_CAMERA_H
