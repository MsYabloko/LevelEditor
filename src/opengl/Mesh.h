
#ifndef LEVELEDITOR_MESH_H
#define LEVELEDITOR_MESH_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
};

class Mesh
{
private:
    unsigned int VAO{}, VBO{}, EBO{};
    void setupMesh(GLuint draw);
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, GLuint draw);
    ~Mesh();
    void draw() const;
};

class EditableMesh
{
public:
    Mesh* _mesh = nullptr;
    EditableMesh();
    ~EditableMesh();
};


#endif //LEVELEDITOR_MESH_H
