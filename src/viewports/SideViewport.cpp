
#include "SideViewport.h"
#include "imgui.h"
#include <cmath>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Project.h"
#include "Fullbright3DViewport.h"

Mesh* SideViewport::_rect;
Shader* SideViewport::_shader;
static Shader* _worldShader = nullptr;

void SideViewport::Render(float x, float y, float width, float height, int id)
{
    if(_shader == nullptr)
    {
        std::vector<Vertex> vertices = {
                {{-1.f, -1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom left
                {{ 1.f, -1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // Bottom right
                {{ 1.f,  1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // Top right
                {{-1.f,  1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}  // Top left
        };

        std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0
        };
        _rect = new Mesh(vertices, indices, GL_STATIC_DRAW);
        _shader = Shader::load("data/siderenderer.vert", "data/siderenderer.frag");
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
    if(_framebuffer == nullptr) return;

    glm::vec3 rightAxis = glm::cross(glm::vec3(0, 1, 0), -lookAxis);
    if(glm::length(rightAxis) == 0) rightAxis = glm::vec3(1, 0, 0);
    glm::vec3 upAxis = glm::cross(-lookAxis, rightAxis);
    rightAxis = glm::abs(rightAxis);

    _camera.right = rightAxis;
    _camera.up = upAxis;

    glm::mat4 view = _camera.GetView();
    glm::mat4 proj = _camera.GetProjection(width, height);

    if(isResized)
    {
        glViewport(0, 0, width, height);
        _framebuffer->use();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);/*
        _shader->use();
        glUniformMatrix4fv(glGetUniformLocation(_shader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(_shader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform2fv(glGetUniformLocation(_shader->getId(), "screensize"), 1, glm::value_ptr(glm::vec2(width, height)));
        _rect->draw();*/
        _worldShader->use();
        glUniformMatrix4fv(glGetUniformLocation(_worldShader->getId(), "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(_worldShader->getId(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        for(auto & mesh : Project::GetCurrent()->meshes)
        {
            mesh._mesh->draw();
        }
        Framebuffer::stop();
    }
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->PushClipRect({x, y}, {x + width, y + height});
    for(auto& gizmo : Project::GetCurrent()->_gizmos)
    {
        gizmo->DrawBefore2D(_camera, x, y, width, height, drawList, id);
    }
    ImGui::SetCursorScreenPos({x, y});
    ImGui::Image(_framebuffer->get_texture(), {width, height});
    if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
    {
        glm::vec2 uv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
        start_pos = _camera.Position;
        start_mouse_pos = uv;
        started_moving = true;
    }
    if(Project::GetCurrent()->CurrentTool != nullptr && ImGui::IsItemHovered())
    {
        glm::vec2 uv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
        glm::vec3 world = _camera.UvToWorld(uv, width, height);
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) Project::GetCurrent()->CurrentTool->OnGridClick(0, world, id);
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) Project::GetCurrent()->CurrentTool->OnGridHold(0, world, id);
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) Project::GetCurrent()->CurrentTool->OnGridUp(0, world, id);
    }
    if (started_moving && ImGui::IsMouseDown(ImGuiMouseButton_Middle))
    {
        glm::vec2 uv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
        glm::vec2 uvdiff = uv - start_mouse_pos;
        uvdiff.x *= (width / height);
        uvdiff *= _camera.Scale / 2.f;
        _camera.Position = start_pos - uvdiff;
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    else
        started_moving = false;
    if(ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel != 0)
    {
        float mouse = ImGui::GetIO().MouseWheel / 10.f;
        _camera.Scale -= _camera.Scale * mouse;
        if(_camera.Scale <= 0.25f) _camera.Scale = 0.25f;
    }


    glm::vec2 lu = _camera.UvToPlanarWorld(glm::vec2(0, 0), width, height);
    glm::vec2 rd = _camera.UvToPlanarWorld(glm::vec2(1, 1), width, height);

    float cell = Project::GetCurrent()->GetCellSize();

    float firstx = ceilf(lu.x / cell) * cell;
    float firsty = ceilf(lu.y / cell) * cell;
    glm::vec2 first = _camera.PlanarWorldToUv(glm::vec2(firstx, firsty), width, height);
    first.x *= width;
    first.y *= height;

    glm::vec2 next = _camera.PlanarWorldToUv(lu + glm::vec2(cell, cell), width, height);
    next.x *= width;
    next.y *= height;

    drawList->AddRectFilled({x, y}, {x + width, y + height}, ImColor(10, 10, 10));
    for(float grid_x = first.x; grid_x < width; grid_x += next.x)
    {
        if(grid_x < 0) continue;
        ImColor color = ImColor(128, 128, 128);
        float line_x = x + grid_x;
        drawList->AddLine({line_x, y}, {line_x, y + height}, color, 2);
    }
    for(float grid_y = first.y; grid_y < height; grid_y += next.x)
    {
        if(grid_y < 0) continue;
        ImColor color = ImColor(128, 128, 128);
        float line_y = y + grid_y;
        drawList->AddLine({x, line_y}, {x + width, line_y}, color, 2);
    }
    glm::vec2 centerdot = _camera.PlanarWorldToUv(glm::vec2(0, 0), width, height);
    centerdot.x *= width;
    centerdot.y *= height;
    if(centerdot.x > 0 && centerdot.x < width)
    {
        float line_x = x + centerdot.x;
        drawList->AddLine({line_x, y}, {line_x, y + height}, ImColor(255 * upAxis.x, 255 * upAxis.y, 255 * upAxis.z), 2);
    }
    if(centerdot.y > 0 && centerdot.y < height)
    {
        float line_y = y + centerdot.y;
        drawList->AddLine({x, line_y}, {x + width, line_y}, ImColor(255 * rightAxis.x, 255 * rightAxis.y, 255 * rightAxis.z), 2);
    }
    Vertex previousVertex{};
    int i = 0;
    for(auto & mesh : Project::GetCurrent()->meshes)
    {
        for(auto& indic : mesh._mesh->indices)
        {
            Vertex current = mesh._mesh->vertices[indic];
            glm::vec2 currentUv = _camera.WorldToUv(current.Position, width, height) * glm::vec2(width, height) + glm::vec2(x, y);
            glm::vec2 prevUv = _camera.WorldToUv(previousVertex.Position, width, height) * glm::vec2(width, height) + glm::vec2(x, y);
            if(i != 0 && i % 3 != 0)
            {
                drawList->AddLine({prevUv.x, prevUv.y}, {currentUv.x, currentUv.y}, ImColor(255, 255, 255));
            }
            previousVertex = current;
            i++;
        }
    }
    int camId = 0;
    for(auto& viewport : Project::GetCurrent()->_viewports)
    {
        if(Fullbright3DViewport* tvp = dynamic_cast<Fullbright3DViewport*>(viewport.GetBehaviour()))
        {
            glm::vec2 uv = _camera.WorldToUv(tvp->camera.Position, width, height);
            uv.x *= width;
            uv.y *= height;
            uv.x += x;
            uv.y += y;
            auto color = ImColor(128, 0, 0);
            if(camId == 1) color = ImColor(0, 128, 0);
            if(camId == 2) color = ImColor(0, 0, 128);
            drawList->AddCircleFilled({uv.x, uv.y}, 250.f / _camera.Scale, color);
            float rightDirection = glm::dot(tvp->camera.get_forward(), rightAxis);
            float upDirection = glm::dot(tvp->camera.get_forward(), upAxis);
            glm::vec2 dirUv = uv + glm::vec2(rightDirection, upDirection) * 500.f / _camera.Scale
                    + glm::normalize(glm::vec2(rightDirection, upDirection)) * 250.f / _camera.Scale;
            drawList->AddLine({uv.x, uv.y}, {dirUv.x, dirUv.y}, color, 64.f / _camera.Scale);
            camId++;
        }
    }
    for(auto& gizmo : Project::GetCurrent()->_gizmos)
    {
        gizmo->Draw2D(_camera, x, y, width, height, drawList, id);
    }
    drawList->PopClipRect();
    ShowCellSize(x, y, width, height, cell);
}

SideViewport::~SideViewport()
{
    delete _framebuffer;
}

SideViewport::SideViewport(glm::vec3 lookAxis)
{
    _framebuffer = nullptr;
    this->lookAxis = lookAxis;
}

