#include "renderer/renderer.h"
#include "core/application.h"
#include "core/window.h"
#include "renderer/batch.hpp"
#include "world/block.h"

namespace SymoCraft{

    Batch<Vertex3D> block_batch;
    namespace Renderer {

        static ShaderProgram block_shader;
        static Camera *camera;

        static glm::mat4 g_projection_mat;
        static glm::mat4 g_view_mat;
        static glm::mat4 g_combo_mat;
        static glm::vec3 g_normal;

        constexpr float depth_value = 1.0f;
        constexpr std::array<float, 4> clear_color = {0.2f, 0.3f, 0.3f, 1.0f};

        // Internal functions
        static void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                               GLsizei length, const GLchar *message, const void *userParam);

        void Init() {
            Window &window = Application::GetWindow();
            camera = Application::GetCamera();

            // Load OpenGL functions using Glad
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                AmoLogger_Error("Failed to initialize glad.\n");
                return;
            }
            std::cout << "GLAD initialized.\n";
            std::cout << "Hello OpenGL " << GLVersion.major << '.' << GLVersion.minor << '\n';

            // glEnable(GL_DEBUG_OUTPUT);
            // glDebugMessageCallback(messageCallback, 0);

            // Enable render parameters
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Initialize shaders
            block_shader.CompileAndLink("../assets/shaders/vs_BlockShader.glsl",
                                        "../assets/shaders/fs_BlockShader.glsl");

            // Initialize batches
            block_batch.init({
                                     {0, 3, GL_INT,   offsetof(Vertex3D, pos_coord)},
                                     {1, 3, GL_FLOAT, offsetof(Vertex3D, tex_coord)},
                                     {2, 3, GL_FLOAT, offsetof(Vertex3D, normal)}});

            loadBlocks("../assets/configs/blockFormats.yaml");

            AmoLogger_Log("Querying grass block id %d\n", get_block_id("grass_block"));
        }

        void Free() {
            block_batch.Free();
            block_shader.Destroy();

            // shader2D.destroy();
            // line3DShader.destroy();
            // regular3DShader.destroy();
            // batch3DVoxelsShader.destroy();
        }

        void Render() {
            ClearBuffers();

            FlushBatches3D();
            // flushBatches2D();
            // flushVoxelBatches();
        }

        void ReloadShaders() {
            // shader2D.destroy();
            // line3DShader.destroy();
            block_shader.Destroy();
            // batch3DVoxelsShader.destroy();

            // shader2D.compile("assets/shaders/DebugShader2D.glsl");
            // line3DShader.compile("assets/shaders/DebugShader3D.glsl");
            block_shader.CompileAndLink("assets/shaders/vs_BlockShader.glsl",
                                        "assets/shaders/fs_BlockShader.glsl");
            // batch3DVoxelsShader.compile("assets/shaders/VoxelShader.glsl");
        }

        void FlushBatches3D() {
            block_shader.Bind();

            g_projection_mat = camera->GetCameraProjMat();
            g_view_mat = camera->GetCameraViewMat();
            g_combo_mat = g_projection_mat * g_view_mat;

            block_shader.UploadMat4("u_combo_mat", g_combo_mat);
            block_batch.Flush();

            block_shader.Unbind();
            // DebugStats::numDrawCalls += 2;
        }

        void FlushBatches3D(const glm::mat4 &projection_mat, const glm::mat4 &view_mat) {
            block_shader.Bind();

            g_combo_mat = projection_mat * view_mat;
            block_shader.UploadMat4("u_combo_mat", g_combo_mat);
            block_batch.Flush();

            block_shader.Unbind();
        }

        void setCamera(const Camera &cameraRef) {
            *camera = cameraRef;
        }

        void ClearBuffers() {
            glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color.data());
            glClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth_value);
        }


        // =========================================================
        // Draw 3D Functions
        // =========================================================

        constexpr std::array<glm::ivec3, 8> k_pos_coords{
                glm::ivec3(0, 0, 0),  // v0
                glm::ivec3(0, 0, 1),  // v1
                glm::ivec3(1, 0, 1),  // v2
                glm::ivec3(1, 0, 0),  // v3
                glm::ivec3(0, 1, 0),  // v4
                glm::ivec3(0, 1, 1),  // v5
                glm::ivec3(1, 1, 1),  // v6
                glm::ivec3(1, 1, 0),  // v7
        };

        // The 8 vertices will look like this:
        //   v4 ----------- v7     v0 is at (0, 0, 0)
        //   /|            /|
        //  / |           / |      Axis orientation
        // v5 --------- v6  |            y
        // |  |         |   |            |
        // |  v0 -------|-- v3           +--- x
        // | /          |  /            /
        // |/           | /            z
        // v1 --------- v2
        //
        // Where v4, v5, v6, v7 is the top face

        // Tex-coords always loop with the triangle going:
        // Counterclockwise order(right-hand rule), starting at top right
        constexpr std::array<glm::vec2, 4> k_tex_coords{
                glm::vec2(1.0f, 1.0f), // top-right
                glm::vec2(0.0f, 1.0f), // top-left
                glm::vec2(0.0f, 0.0f), // bottom-left
                glm::vec2(1.0f, 0.0f), // bottom-right
        };

        constexpr std::array<uint16, 24> k_vertex_indices = {
                // Each set of 6 indices represents one quad
                7, 6, 2, 3, // Front face
                6, 5, 1, 2, // Right face
                5, 4, 0, 1, // Back face
                4, 7, 3, 0, // Left face
                7, 4, 5, 6, // Top face
                2, 1, 0, 3  // Bottom face
        };

        static std::array<std::array<Vertex3D, 4>, 6> block_faces{}; // Each block contains 6 faces, which contains 4 vertices

        static uint16 index;
        static uint16 vertex_count{0};
        static uint16 block_count{0};

        void AddBlocksToBatch(const glm::ivec3 &block_center_coord, const uint16 &side_tex, const uint16 &top_tex,
                              const uint16 &bottom_tex) {
            // Let Amo decide what value should the normal have...
            // glm::vec3 normal = glm::vec3(offset.x, offset.y, offset.z);
            g_normal = glm::vec3();

            for (index = 0; auto &face: block_faces) {
                for (auto &vertex: face) {
                    vertex.pos_coord = (k_pos_coords[k_vertex_indices[index]] + block_center_coord);
                    vertex.tex_coord = {k_tex_coords[index % 4], // Set uv coords
                                        (index >= 16) ? // Set layer index, sides first, the top second, the bottom last
                                        ((index >= 20) ? bottom_tex : top_tex) // if 16 <= index < 20, assign top_tex
                                                      : side_tex}; // if index < 16, assign side_tex
                    vertex.normal = g_normal;
                    index++;
                }

                // Add the block's top left triangle
                block_batch.AddVertex(face[0]);
                block_batch.AddVertex(face[1]);
                block_batch.AddVertex(face[2]);

                // Add the block's bottom right triangle
                block_batch.AddVertex(face[0]);
                block_batch.AddVertex(face[2]);
                block_batch.AddVertex(face[3]);
            }
        }

        void ReportStatus() {
            if (block_count % 10000 == 0)
                AmoLogger_Log("%d vertices, %d blocks in total loaded\n", vertex_count, block_count);
        }

        // =========================================================
        // Internal Functions
        // =========================================================
        // static void GLAPIENTRY
        // messageCallback(GLenum source, GLenum type, GLuint id,
        //                 GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
        // {
        //     if (type == GL_DEBUG_TYPE_ERROR)
        //     {
        //         std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
        //                   << "type = 0x" << type
        //                   << "severity = 0x" << severity
        //                   << "message = " << message;
        //
        //         GLenum err;
        //         while ((err = glGetError()) != GL_NO_ERROR)
        //         {
        //             std::cerr << "Error Code: " << err;
        //         }
        //     }
        // }
    }
}
