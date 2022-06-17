#pragma once
#include "core.h"
#include "shader_program.h"
#include "camera/camera.h"

namespace SymoCraft{
    struct Framebuffer;
    struct DrawArraysIndirectCommand
    {
        uint32 count;
        uint32 instanceCount;
        uint32 first;
        uint32 baseInstance;
    };

    extern uint16 vertex_count;
    extern uint16 face_count;

    namespace Renderer
    {
        void Init();
        void Free();
        void Render();

        void reloadShaders();

        // void flushBatches2D();
        void FlushBatches3D();
        void FlushBatches3D(const glm::mat4& projection_mat, const glm::mat4& view_mat);
        // void flushVoxelBatches();

        // void setShader2D(const Shader& shader);
        void setShader(const Shader& shader);
        void setCamera(const Camera& camera);
        //void setCameraFrustum(const Frustum& cameraFrustum);

        void ClearBuffers();

        // 2D Rendering stuff
        // void drawSquare2D(const glm::vec2& start, const glm::vec2& size, const Style& style, int zIndex = 0);
        // void drawFilledSquare2D(const glm::vec2& start, const glm::vec2& size, const Style& style, int zIndex = 0);
        // void drawLine2D(const glm::vec2& start, const glm::vec2& end, const Style& style, int zIndex = 0);
        // void drawFilledCircle2D(const glm::vec2& position, float radius, int numSegments, const Style& style, int zIndex = 0);
        // void drawFilledTriangle2D(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const Style& style, int zIndex = 0);
        // void drawTexture2D(const Sprite& sprite, const glm::vec2& position, const glm::vec2& size, const Style& color, int zIndex = 0, bool isFont = false);
        // void drawString(const std::string& string, const Font& font, const glm::vec2& position, float scale, const Style& style, int zIndex = 0);

        // 3D Rendering stuff
        // void draw3DModel(const glm::vec3& position, const glm::vec3& scale, float rotation, const VoxelVertex* vertices, int verticesLength);
        // void drawLine(const glm::vec3& start, const glm::vec3& end, const Style& style);
        // void drawBox(const glm::vec3& block_center_coord, const glm::vec3& size, const Style& style);
        void AddBlocksToBatch(const glm::ivec3 &block_center_coord, const uint16 &side_tex, const uint16 &top_tex, const uint16 &bottom_tex);

        void ReportStatus();

    }

}