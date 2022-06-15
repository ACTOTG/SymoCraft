#include "renderer/renderer.h"
#include "Core/application.h"
#include "Core/window.h"

namespace SymoCraft{

    namespace Renderer
    {
        static Batch<Vertex3D> block_batch;

        static Camera* camera;
        static ShaderProgram block_shader;

        static uint16 vertex_count{0};
        static uint16 block_count{0};

        constexpr static GLfloat depth_value = 1.0f;
        constexpr static std::array<float, 4> clear_color = { 0.2f, 0.3f, 0.3f, 1.0f };

        // Internal functions
        static void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                               GLsizei length, const GLchar* message, const void* userParam);

        void Init()
        {
            Window &window = Application::GetWindow();
            camera = new Camera(window.width, window.height);

            // Load OpenGL functions using Glad
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cerr << "Failed to initialize glad.\n";
                return;
            }
            std::cout << "GLAD initialized.\n" ;
            std::cout << "Hello OpenGL " << GLVersion.major << '.' << GLVersion.minor << '\n';

#ifdef _DEBUG
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(messageCallback, 0);
#endif

            // Enable render parameters
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Initialize shaders
            block_shader.CompileAndLink("assets/shaders/vs_BlockShader.glsl",
                                        "assets/shaders/fs_BlockShader.glsl");

            // Initialize batches
            block_batch.init(
                    {
                        {0, 3, offsetof(Vertex3D, pos_coord)},
                        {1, 3, offsetof(Vertex3D, tex_coord)},
                        {2, 3, offsetof(Vertex3D, normal)}
                    } );

        }

        void Free()
        {
            block_batch.Free();
            block_shader.Destroy();

            // shader2D.destroy();
            // line3DShader.destroy();
            // regular3DShader.destroy();
            // batch3DVoxelsShader.destroy();
        }

        void Render()
        {
            FlushBatches3D();
            // flushBatches2D();
            // flushVoxelBatches();
        }

        void ReloadShaders()
        {
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

        void FlushBatches3D()
        {
            block_shader.Bind();

            glm::mat4 model_mat = glm::mat4(1.0f);
            glm::mat4 view_mat = glm::mat4(1.0f);
            glm::mat4 projection_mat = glm::mat4(1.0f);
            glm::mat4 combo_mat = glm::mat4(1.0f);

            model_mat = glm::rotate(model_mat, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            projection_mat = camera->GetCameraProjMat();
            view_mat = camera->GetCameraViewMat();
            combo_mat = projection_mat * view_mat * model_mat;

            block_shader.UploadMat4("u_combo_mat", combo_mat);
            block_batch.Flush();

            // DebugStats::numDrawCalls += 2;
        }

        void FlushBatches3D(const glm::mat4& projection_mat, const glm::mat4& view_mat)
        {
            block_shader.Bind();

            glm::mat4 combo_mat = projection_mat * view_mat;
            block_shader.UploadMat4("u_combo_mat", combo_mat);
            block_batch.Flush();

            block_shader.Unbind();
        }

        void setCamera(const Camera &cameraRef)
        {
            *camera = cameraRef;
        }

        void ClearBuffers()
        {
            glClearNamedFramebufferfv(0, GL_COLOR, 0, clear_color.data());
            glClearNamedFramebufferfv(0, GL_DEPTH, 0, &depth_value);
        }


        // =========================================================
        // Draw 3D Functions
        // =========================================================
        void AddBlocksToBatch(const glm::vec3 &center, const uint16 &side_tex, const uint16 &top_tex,
                              const uint16 &bottom_tex)
        {
            //if (cameraFrustum && !cameraFrustum->isBoxVisible(center - halfSize, center + halfSize))
            //{
            //    return;
            //}

            // BlockVertices

            const std::array<glm::vec3, 8> pos_coords{      // (0, 0, 0) is at the center of the block
                glm::vec3(-0.5f,  0.5f,  0.5f), // v0
                glm::vec3( 0.5f,  0.5f,  0.5f), // v1
                glm::vec3(-0.5f, -0.5f,  0.5f), // v2
                glm::vec3( 0.5f, -0.5f,  0.5f), // v3
                glm::vec3(-0.5f,  0.5f, -0.5f), // v4
                glm::vec3( 0.5f,  0.5f, -0.5f), // v5
                glm::vec3(-0.5f, -0.5f, -0.5f), // v6
                glm::vec3( 0.5f, -0.5f, -0.5f), // v7
            };

            // The 8 vertices will look like this:
            //   v4 ----------- v5
            //   /|            /|      Axis orientation
            //  / |           / |
            // v0 --------- v1  |      y
            // |  |         |   |      |
            // |  v6 -------|-- v7     +--- x
            // | /          |  /      /
            // |/           | /      z
            // v2 --------- v3
            //
            // Where v0, v4, v5, v1 is the top face

            // Tex-coords always loop with the triangle going:
            const std::array<glm::vec2, 4> tex_coords{
                glm::vec2(0.0f, 1.0f), // top-left
                glm::vec2(1.0f, 1.0f), // top-right
                glm::vec2(0.0f, 0.0f), // bottom-left
                glm::vec2(1.0f, 0.0f), // bottom-right
            };

            const std::array<uint16, 24> vertex_indices = {
                // Each set of 6 indices represents one quad
                1, 0, 2, 3, // Front face
                5, 1, 3, 7, // Right face
                4, 5, 7, 6, // Back face
                0, 4, 6, 2, // Left face
                5, 4, 0, 1, // Top face
                3, 2, 6, 7  // Bottom face
            };

            // Let Amo decide what value should the normal have...
            // glm::vec3 normal = glm::vec3(offset.x, offset.y, offset.z);
            glm::vec3 normal = glm::vec3();
            glm::mat4 model_mat = glm::translate(glm::mat4(1.0f), center);

            std::array<std::array<Vertex3D, 4>, 6> block_faces{}; // Each block contains 6 faces, which contains 4 vertices

            uint16 index = 0;
            for (auto &face : block_faces)
            {
                for(auto &vertex : face)
                {
                    vertex.pos_coord = (model_mat * glm::vec4(pos_coords[ vertex_indices[index] ], 1.0f) ).xyz();
                    vertex.tex_coord = {tex_coords[ vertex_indices[index % 4] ], // Set uv coords
                                        (index >= 16) ? // Set layer index, sides first, the top second, the bottom last
                                        ( (index >= 20) ? bottom_tex : top_tex ) // if 16 <= index < 20, assign top_tex
                                        : side_tex }; // if index < 16, assign side_tex
                    vertex.normal = normal;
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

        void ReportStatus(){
            std::cout << vertex_count << " vertices in total loaded\n"
                      << block_count << " blocks in total loaded\n";
        }
        // =========================================================
        // Internal Functions
        // =========================================================
        static void GLAPIENTRY
        messageCallback(GLenum source, GLenum type, GLuint id,
                        GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
        {
            if (type == GL_DEBUG_TYPE_ERROR)
            {
                std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
                          << "type = 0x" << type
                          << "severity = 0x" << severity
                          << "message = " << message;

                GLenum err;
                while ((err = glGetError()) != GL_NO_ERROR)
                {
                    std::cerr << "Error Code: " << err;
                }
            }
        }
    }
}