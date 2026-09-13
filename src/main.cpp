#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "ViewPort.h"
#include "Project.h"
#include "tools/BlockTool.h"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Level Editor", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void )io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    Project::CreateNew();

    while(!glfwWindowShouldClose(window))
    {
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::IsKeyPressed(ImGuiKey_RightBracket))
        {
            Project* _project = Project::GetCurrent();
            _project->cell_multiplier++;
        }
        if(ImGui::IsKeyPressed(ImGuiKey_LeftBracket))
        {
            Project* _project = Project::GetCurrent();
            _project->cell_multiplier--;
        }

        //Start of ImGui Windows segments

        ImGui::ShowDemoWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::SetNextWindowSize(ImVec2(display_w + 2, display_h + 1));
        ImGui::SetNextWindowPos(ImVec2(-1,0));
        ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration
         | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove
         | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(30, 30, 30).Value);
        ImGui::BeginChildFrame(2, {static_cast<float>(display_w), 25});
        ImGui::PopStyleColor();

        if(Project::GetCurrent()->CurrentTool != nullptr) Project::GetCurrent()->CurrentTool->RenderToolbox();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
        ImGui::EndChildFrame();
        ImGui::PopStyleVar();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(30, 30, 30).Value);
        ImGui::BeginChildFrame(1, ImVec2(50, display_h));
        ImGui::PopStyleColor();

        struct ToolButton
        {
            std::string Name;
            std::function<BaseTool*()> _tool;
        };
        static const ToolButton _tools[] = {
                {"Selection", [](){return nullptr;}},
                {"Block", [](){return new BlockTool();}},
                {"Entity", [](){return nullptr;}}
        };

        static int selectedTool = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor(87, 87, 87).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(110, 110, 110).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(137, 137, 137).Value);
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(162, 162, 162).Value);

        int toolN = 0;
        for(const auto& tool : _tools)
        {
            int prevSelected = selectedTool;
            if(toolN == prevSelected) ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
            if(ImGui::Button(tool.Name.c_str(), {50 - 7, 50 - 7}))
            {
                delete Project::GetCurrent()->CurrentTool;
                Project::GetCurrent()->CurrentTool = tool._tool();
                selectedTool = toolN;
            }
            if(toolN == prevSelected) ImGui::PopStyleVar();
            toolN++;
        }
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();

        ImGui::EndChildFrame();

        ImGui::PopStyleVar();

        //ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        //ImGui::SameLine();
        //ImGui::PopStyleVar();

        const float min_w = 49;
        const float min_h = 19 + 25;
        const float max_w = (float )display_w;
        const float max_h = (float)display_h;

        static float width = ((float)display_w - min_w) / 2.f;
        static float height = ((float)display_h - min_h) / 2.f;
        const float splitter_width = 5;
        const float min_splitter = 25;

        const float center_size = splitter_width * 2;
        ImGui::SetCursorScreenPos({min_w + width - splitter_width / 2.f, height + min_h - splitter_width / 2.f});
        ImGui::InvisibleButton("all_resize", {center_size, center_size});
        bool isCentering = ImGui::IsItemHovered() || ImGui::IsItemActive();
        if(isCentering)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        if(ImGui::IsItemActive())
        {
            width = ImGui::GetMousePos().x - min_w;
            height = ImGui::GetMousePos().y - min_h;
        }

        if(!isCentering)
        {
            ImGui::SetCursorScreenPos({min_w + width, min_h});
            ImGui::InvisibleButton("width_resize", {splitter_width, max_h - min_w});
            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            if (ImGui::IsItemActive())
                width = ImGui::GetMousePos().x - min_w;

            ImGui::SetCursorScreenPos({min_w, height + min_h});
            ImGui::InvisibleButton("height_resize", {max_w - min_w, splitter_width});
            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (ImGui::IsItemActive())
                height = ImGui::GetMousePos().y - min_h;
        }

        if(width < 25) width = 25; if(width > max_w - min_w - min_splitter) width = max_w - min_w - min_splitter;
        if(height < 25) height = 25; if(height > max_h - min_h - min_splitter) height = max_h - min_h - min_splitter;

        Project::GetCurrent()->_gizmos.clear();
        if(Project::GetCurrent()->CurrentTool != nullptr) Project::GetCurrent()->CurrentTool->Update();

        Project::GetCurrent()->_viewports[0].Render(min_w, min_h, width, height);
        Project::GetCurrent()->_viewports[1].Render(min_w + width, min_h, max_w - min_w - width, height);
        Project::GetCurrent()->_viewports[2].Render(min_w, min_h + height, width, max_h - min_h - height);
        Project::GetCurrent()->_viewports[3].Render(min_w + width, min_h + height, max_w - min_w - width, max_h - min_h - height);

        if(!Project::GetCurrent()->GetActionName().empty())
        {
            static bool isActionCollapsing;
            float actionWidth = 150;
            float actionHeight = isActionCollapsing ? 200 : (ImGui::GetTextLineHeight() + 6);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(45, 45, 45).Value);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
            glm::vec2 actionWindowStart = {min_w + 5, max_h - actionHeight - 5};
            ImGui::SetCursorScreenPos({actionWindowStart.x, actionWindowStart.y});
            bool actionOpened = ImGui::BeginChildFrame(99, {actionWidth, actionHeight});
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            if (actionOpened)
            {
                ImDrawList* actionList = ImGui::GetWindowDrawList();
                ImGui::PushStyleColor(ImGuiCol_Header, ImColor(0,0,0,0).Value);
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImColor(0,0,0,0).Value);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImColor(0,0,0,0).Value);
                actionList->AddRectFilled({actionWindowStart.x, actionWindowStart.y},
                                          {actionWindowStart.x + actionWidth, actionWindowStart.y + ImGui::GetTextLineHeight() + 6},
                                          ImColor(55, 55, 55), 5.f, ImDrawFlags_RoundCornersTop);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
                isActionCollapsing = ImGui::CollapsingHeader((Project::GetCurrent()->GetActionName() + "##ActionMenuHeader").c_str());
                ImGui::PopStyleVar();
                if(isActionCollapsing)
                {

                    Project::GetCurrent()->RenderAction();
                }
                ImGui::PopStyleColor(3);
                ImGui::EndChildFrame();
            }
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine({min_w + width, min_h}, {min_w + width, max_h},
                          ImColor(100, 100, 100), splitter_width / 2.f);
        drawList->AddLine({min_w, min_h + height}, {max_w, min_h + height},
                          ImColor(100, 100, 100), splitter_width / 2.f);

        ImGui::End();

        //End

        //Start of other Rendering

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, display_w, display_h);

        //End

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
