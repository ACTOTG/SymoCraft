//
// Created by Amo on 2022/6/12.
//

#ifndef SYMOCRAFT_CAMERA_H
#define SYMOCRAFT_CAMERA_H
#include "core.h"


namespace SymoCraft {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum MoveDirection {
        FORWARD = 0,
        BACKWARD,
        LEFT,
        RIGHT,
        UPWARD,
        DOWNWARD
    };

    struct Transform;

    namespace ECS
    {
        typedef uint64 EntityId;
    }

    class Camera {
    public:

        ECS::EntityId entity_id;
        // Constructor.
        // Parameters: Viewport width, Viewport height, camera Position;
        Camera(float, float, glm::vec3 = glm::vec3(0.0f, 0.0f, 0.0f));

        //--------------------------------------------------------------------------------
        // The callback functions

        // Mouse movement callback for GLFW window
        // Parameters: GLFW window Ptr, mouse xPosIn, mouse yPosIn
        void InsMouseMovementCallBack(GLFWwindow *, double, double);

        // Mouse scroll callback for GLFW window
        // Parameters: GLFW window Ptr, mouse wheel xPosIn, mouse wheel yPosIn
        void InsMouseScrollCallBack(GLFWwindow *, double, double);

        // --------------------------------------------------------------------------------
        // Camera move
        // Parameters: move direction, move displacement
        void CameraMoveBy(MoveDirection, float);


        // --------------------------------------------------------------------------------
        // Matrix Functions

        // Return View Matrix
        glm::mat4 GetCameraViewMat() const;

        // Return Projection Matrix
        // Parameters: window aspect ratio
        glm::mat4 GetCameraProjMat() const;

        // --------------------------------------------------------------------------------
        // private data member functions


        glm::vec3 GetCameraPos() const;      // return cameraPos
        glm::vec2 GetCameraPos_vec2() const;      // return cameraPos

        void SetCameraPos(glm::vec3& );      // set cameraPos

        float GetYaw() const;                // return yaw
        void SetYaw( float&);                // set yaw

        float GetPitch() const;              // return pitch
        void SetPitch( float&);              // set pitch

        float GetFov() const;                // return fov
        glm::vec3 GetCameraFront() const;    // return camera_front
        glm::vec3 GetCameraUp() const;       // return camera_up;


    private:
        static const glm::vec3 world_up;

        float fov;          // field of view

    }; // camera


}

#endif //SYMOCRAFT_CAMERA_H
