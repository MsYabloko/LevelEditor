
#include <glm/gtc/type_ptr.hpp>
#include "Fullbright3DViewport.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "../Project.h"
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/intersect.hpp"

Mesh* Fullbright3DViewport::_mesh;
Shader* Fullbright3DViewport::_shader;
Shader* Fullbright3DViewport::_worldShader;

void Fullbright3DViewport::Render(float x, float y, float width, float height, int id)
{
    if(_shader == nullptr)
    {
        std::vector<Vertex> vertices = {
                {{-1.f, -0.f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom left
                {{ 1.f,  0.f, -1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // Bottom right
                {{ 1.f,  0.f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // Top right
                {{-1.f,  0.f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}  // Top left
        };

        std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0
        };
        _mesh = new Mesh(vertices, indices, GL_STATIC_DRAW);
        _shader = Shader::load("data/infinitegrid.vert", "data/infinitegrid.frag");
        _worldShader = Shader::load("data/world.vert", "data/world.frag");
    }
    if(!isResized && width == prev_width && height == prev_height)
    {
        isResized = true;
        delete _framebuffer;
        _framebuffer = new Framebuffer(width, height);
    }
    if(width != prev_width || height != prev_height)
    {
        prev_width = width;
        prev_height = height;
        isResized = false;
    }
    camera.fov = 90;
    camera.update_directions();
    if(_framebuffer == nullptr) return;
    float cellSize = Project::GetCurrent()->GetCellSize();
    if(isResized)
    {
        glViewport(0, 0, width, height);
        _framebuffer->use();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _shader->use();
        glUniformMatrix4fv(glGetUniformLocation(_shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.get_projection(width, height)));
        glUniformMatrix4fv(glGetUniformLocation(_shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
        glUniform3fv(glGetUniformLocation(_shader->getId(), "cameraWorldPos"), 1, glm::value_ptr(camera.Position));
        glUniform1fv(glGetUniformLocation(_shader->getId(), "gGridCellSize"), 1, &cellSize);
        _mesh->draw();
        _worldShader->use();
        glUniformMatrix4fv(glGetUniformLocation(_worldShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(camera.get_projection(width, height)));
        glUniformMatrix4fv(glGetUniformLocation(_worldShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(camera.get_view()));
        for(auto & mesh : Project::GetCurrent()->meshes)
        {
            mesh._mesh->draw();
        }
        for(auto& gizmo : Project::GetCurrent()->_gizmos)
        {
            gizmo->Draw3D(camera, width, height);
        }
        Framebuffer::stop();
    }
    ImGui::SetCursorScreenPos({x, y});
    ImGui::Image(_framebuffer->get_texture(), {width, height});
    if(Project::GetCurrent()->CurrentTool != nullptr && ImGui::IsItemHovered())
    {
        glm::vec2 uv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
        glm::vec3 camCoords = glm::unProject(glm::vec3(uv.x, uv.y, 1.f), camera.get_view(), camera.get_projection(width, height),
                                             glm::vec4(0, 0, 1.f, 1.f));
        glm::vec3 dir = glm::normalize(camCoords - camera.Position);
        float intersectionDistance = 0;
        bool intersectRayPlane = glm::intersectRayPlane(camera.Position, dir, glm::vec3(0,0,0), glm::vec3(0, 1, 0), intersectionDistance);
        if(intersectRayPlane && intersectionDistance > 0)
        {
            glm::vec3 worldPos = camera.Position + dir * intersectionDistance;
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                Project::GetCurrent()->CurrentTool->OnGridClick(0, worldPos, id);
            }
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                Project::GetCurrent()->CurrentTool->OnGridHold(0, worldPos, id);
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                Project::GetCurrent()->CurrentTool->OnGridUp(0, worldPos, id);
            }
        }
    }
    if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        start_mouse.x = ImGui::GetMousePos().x;
        start_mouse.y = ImGui::GetMousePos().y;
        start_rotation = camera.Rotation;
        started_moving = true;
    }
    if (started_moving && ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        float delta = glfwGetTime() - prev_time;
        prev_time = glfwGetTime();
        glm::vec2 mouse{};
        mouse.x = ImGui::GetMousePos().x;
        mouse.y = ImGui::GetMousePos().y;
        glm::vec2 offset = mouse - start_mouse;
        camera.Rotation = start_rotation + glm::vec3(offset.y, offset.x, 0) / 2.f;
        if(camera.Rotation.x >= 90) camera.Rotation.x = 89.9;
        if(camera.Rotation.x <= -90) camera.Rotation.x = -89.9;
        glm::vec3 move{};
        if(ImGui::IsKeyDown(ImGuiKey_W)) move += camera.get_forward();
        if(ImGui::IsKeyDown(ImGuiKey_S)) move -= camera.get_forward();
        if(ImGui::IsKeyDown(ImGuiKey_D)) move += camera.get_right();
        if(ImGui::IsKeyDown(ImGuiKey_A)) move -= camera.get_right();
        if(glm::length(move) > 0)
            move = glm::normalize(move) * delta * 10.f;
        camera.Position += move;
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    else
        started_moving = false;
    ShowCellSize(x, y, width, height, cellSize);
}

Fullbright3DViewport::~Fullbright3DViewport()
{
    delete _framebuffer;
}

Fullbright3DViewport::Fullbright3DViewport()
{
    camera.Position = {0, -5, 0};
    camera.Rotation = {0, -90, 0};
}

