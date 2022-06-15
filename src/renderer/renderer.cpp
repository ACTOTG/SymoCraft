#include "renderer/renderer.h"
#include "core/application.h"
#include "core/window.h"

namespace SymoCraft::Renderer {

        static Batch<Vertex3D> block_batch;
        static ShaderProgram block_shader;
        static Camera *camera;

        static glm::mat4 g_projection_mat;
        static glm::mat4 g_view_mat;
        static glm::mat4 g_combo_mat;
        static glm::mat4 g_model_mat;
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
                        {0, 3, offsetof(Vertex3D, pos_coord)},
                        {1, 3, offsetof(Vertex3D, tex_coord)},
                        {2, 3, offsetof(Vertex3D, normal)}     });
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
            glEnable(GL_DEPTH_TEST);
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

        constexpr std::array<glm::vec3, 8> k_pos_coords{  // (0, 0, 0) is the center of the block
                glm::vec3(-0.5f,  0.5f,  0.5f),  // v0
                glm::vec3( 0.5f,  0.5f,  0.5f),  // v1
                glm::vec3(-0.5f, -0.5f,  0.5f),  // v2
                glm::vec3( 0.5f, -0.5f,  0.5f),  // v3
                glm::vec3(-0.5f,  0.5f, -0.5f),  // v4
                glm::vec3( 0.5f,  0.5f, -0.5f),  // v5
                glm::vec3(-0.5f, -0.5f, -0.5f),  // v6
                glm::vec3( 0.5f, -0.5f, -0.5f),  // v7
        };

        // The 8 vertices will look like this:
        //   v4 ----------- v5
        //   /|            /|
        //  / |           / |      Axis orientation
        // v0 --------- v1  |            y
        // |  |         |   |            |
        // |  v6 -------|-- v7           +--- x
        // | /          |  /            /
        // |/           | /            z
        // v2 --------- v3
        //
        // Where v0, v4, v5, v1 is the top face

        // Tex-coords always loop with the triangle going:
        constexpr std::array<glm::vec2, 4> k_tex_coords{
                glm::vec2(0.0f, 1.0f), // top-left
                glm::vec2(1.0f, 1.0f), // top-right
                glm::vec2(0.0f, 0.0f), // bottom-left
                glm::vec2(1.0f, 0.0f), // bottom-right
        };

        constexpr std::array<uint16, 24> k_vertex_indices = {
                // Each set of 6 indices represents one quad
                1, 0, 2, 3, // Front face
                5, 1, 3, 7, // Right face
                4, 5, 7, 6, // Back face
                0, 4, 6, 2, // Left face
                5, 4, 0, 1, // Top face
                3, 2, 6, 7  // Bottom face
        };

        static std::array<std::array<Vertex3D, 4>, 6> block_faces{}; // Each block contains 6 faces, which contains 4 vertices

        static uint16 index;
        static uint16 vertex_count{0};
        static uint16 block_count{0};

        void AddBlocksToBatch(const glm::vec3 &block_center_coord, const uint16 &side_tex, const uint16 &top_tex,
                              const uint16 &bottom_tex) {
            index = 0;
            // Let Amo decide what value should the normal have...
            // glm::vec3 normal = glm::vec3(offset.x, offset.y, offset.z);
            g_normal = glm::vec3();
            g_model_mat = glm::translate(glm::mat4(1.0f), block_center_coord);

            for (auto &face: block_faces) {
                for (auto &vertex: face) {
                    vertex.pos_coord = (g_model_mat * glm::vec4(k_pos_coords[k_vertex_indices[index]], 1.0f)).xyz();
                    vertex.tex_coord = {k_tex_coords[k_vertex_indices[index % 4]], // Set uv coords
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

                vertex_count += 6;
            }
            block_count += 1;
        }

        void ReportStatus() {
            if (sin(glfwGetTime()) == 1)
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