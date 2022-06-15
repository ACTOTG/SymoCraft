//
// Created by Amo on 2022/6/12.
//

#ifndef SYMOCRAFT_CAMERA_H
#define SYMOCRAFT_CAMERA_H
#include "core.h"

namespace SymoCraft
{

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum MoveDirection {
        FORWARD = 0,
        BACKWARD,
        LEFT,
        RIGHT,
        UPWARD,
        DOWNWARD
    };

    class Camera{
    public:

        // Constructor.
        // Parameters: Viewport width, Viewport height, camera Position;
        Camera( float, float, glm::vec3 = glm::vec3( 0.0f, 0.0f, 0.0f));

        //--------------------------------------------------------------------------------
        // The callback functions

        // Mouse movement callback for GLFW window
        // Parameters: GLFW window Ptr, mouse xPosIn, mouse yPosIn
        void InsMouseMovementCallBack(GLFWwindow* , double, double);

        // Mouse scroll callback for GLFW window
        // Parameters: GLFW window Ptr, mouse wheel xPosIn, mouse wheel yPosIn
        void InsMouseScrollCallBack(GLFWwindow*, double, double);

        // --------------------------------------------------------------------------------
        // Camera move
        // Parameters: move direction, move displacement
        void CameraMoveBy( MoveDirection, float);


        // --------------------------------------------------------------------------------
        // Matrix Functions

        // Return View Matrix
        glm::mat4 GetCameraViewMat() const;

        // Return Projection Matrix
        // Parameters: window aspect ratio
        glm::mat4 GetCameraProjMat() const;

        // --------------------------------------------------------------------------------
        // private data member functions

        inline glm::vec3 GetCameraPos() const;      // return cameraPos
        inline void SetCameraPos(glm::vec3& );      // set cameraPos

        inline float GetYaw() const;                // return yaw
        inline void SetYaw( float&);                // set yaw

        inline float GetPitch() const;              // return pitch
        inline void SetPitch( float&);              // set pitch

        inline float GetFov() const;                // return fov
        inline glm::vec3 GetCameraFront() const;    // return camera_front
        inline glm::vec3 GetCameraUp() const;       // return camera_up;

    private:
        static const glm::vec3 world_up;
        glm::vec3 camera_pos;     // camera position in world space

        glm::vec3 camera_front;   // camera front direction
        glm::vec3 camera_right;   // camera right direction
        glm::vec3 camera_up;      // camera up direction

        float yaw;          // camera's yaw angle
        float pitch;        // camera's pitch angle
        float fov;          // field of view
        float last_x;       // last x position of cursor
        float last_y;       // last y position of cursor
        bool first_enter = true;  // is first enter of cursor? initialized by true

        void UpdateCameraVectors();
    }; // camera

    inline glm::vec3 Camera::GetCameraPos() const
    {
        return camera_pos;
    }

    inline void Camera::SetCameraPos(glm::vec3& c_pos)
    {
        camera_pos = c_pos;
    }

    inline float Camera::GetYaw() const
    {
        return yaw;
    }

    inline void Camera::SetYaw(float &_yaw)
    {
        yaw = _yaw;
    }

    inline float Camera::GetPitch() const
    {
        return pitch;
    }

    inline void Camera::SetPitch(float &_pitch)
    {
        pitch = _pitch;
    }

    inline float Camera::GetFov() const
    {
        return fov;
    }

    inline glm::vec3 Camera::GetCameraFront() const
    {
        return camera_front;
    }

    inline glm::vec3 Camera::GetCameraUp() const
    {
        return camera_up;
    }

}
#endif //SYMOCRAFT_CAMERA_H
