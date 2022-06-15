#pragma once
#include "Core.h"

namespace SymoCraft{

    //Batch Procedure:
    //I.Initialization
    //  1. Create and bind buffers (full DSA)
    //  2. Allocate maximum batch memory to the VBO
    //  3. Configure vertex attributes
    //II.Feed 'data' with vertices
    //  1. Check errors
    //  2. Feed 'data'
    //  3. Tick index
    //III.Draw all vertices in the memory
    //  1. Deliver data to VBO memory
    //  2. Draw vertices
    //  3. Clear the batch

    struct Vertex3D{
        glm::vec3 pos_coord;
        glm::vec3 tex_coord;
        glm::vec3 normal;
    };

    struct VertexAttribute{
        int attribute_slot;
        int element_amount;
        uint32 offset;
    };

    constexpr uint32 kMaxBatchSize = 10000;

    template<typename T>
    class Batch
    {
    public:
        void init(std::initializer_list<VertexAttribute> vertex_attributes)
        {
            m_data_size = sizeof(T) * kMaxBatchSize;
            data = new T[kMaxBatchSize];

            // Create buffers
            glCreateBuffers(1, &m_vbo);
            glCreateVertexArrays(1, &m_vao);

            // Allocate memory for the VBO, and bind the buffers
            glNamedBufferStorage(m_vbo, m_data_size, nullptr, GL_DYNAMIC_STORAGE_BIT);
            glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(Vertex3D));


            // Configure vertex attributes
            // Draw float data solely for now
            // Add support for other data in the future
            for (const auto& attribute : vertex_attributes)
            {
                glEnableVertexArrayAttrib(m_vao, attribute.attribute_slot);
                glVertexArrayAttribFormat(m_vao, attribute.attribute_slot, attribute.element_amount, GL_FLOAT, GL_FALSE, attribute.offset);
                glVertexArrayAttribBinding(m_vao, attribute.attribute_slot, 0);
            }

            zIndex = 0;
            m_vertex_amount = 0;
        }

        void AddVertex(const T& vertex)
        {
            if(!data)
                std::cerr << "Invalid batch.\n";
            if (!hasRoom())
            {
                std::cerr << "Batch ran out of room. I have " << m_vertex_amount << '/' << kMaxBatchSize << " vertices.\n";
                return;
            }
            if (m_vertex_amount < 0)
            {
                std::cerr << "Invalid vertex number.\n";
                return;
            }

            data[m_vertex_amount] = vertex;
            m_vertex_amount++;
        }

        void Flush()  //Draw vertices
        {
            if (m_vertex_amount <= 0)
            {
                std::cerr << "No vertices to draw.\n";
                return;
            }

            // Deliver data to VBO memory
            glNamedBufferSubData(m_vbo, 0, m_data_size, data);

            // Draw the 3D screen space stuff
            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLES, 0, m_vertex_amount);
            glBindVertexArray(0);

            // Clear the batch
            m_vertex_amount = 0;
        }

        void Free()
        {
            if (data)
            {
                delete data;
                data = nullptr;
                m_data_size = 0;
            }
        }

        bool hasRoom() const
        {
            return m_vertex_amount <= kMaxBatchSize;
        }

        bool operator<(const Batch& batch) const
        {
            return (zIndex < batch.zIndex);
        }

    private:
        uint32 m_vao;
        uint32 m_vbo;
        uint32 m_vertex_amount;
        int32 zIndex;
        uint32 m_data_size;
        T* data;
    };

}