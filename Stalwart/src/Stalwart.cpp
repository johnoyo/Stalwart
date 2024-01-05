#include "Stalwart.h"

#include "glm/gtc/type_ptr.hpp"

void Stalwart::OnAttach()
{
    Meek::Random::Init();

    m_Camera = Camera(45.0f, 0.1f, 100.0f);

    Material& greenSphere = m_Scene.Materials.emplace_back();
    greenSphere.Albedo = { 0.35f, 1.0f, 0.0f };
    greenSphere.Roughness = 0.85f;

    Material& blueSphere = m_Scene.Materials.emplace_back();
    blueSphere.Albedo = { 0.0f, 0.2f, 1.0f };
    blueSphere.Roughness = 0.05f;

    Material& emissiveSphere = m_Scene.Materials.emplace_back();
    emissiveSphere.Albedo = { 0.8f, 0.5f, 0.2f };
    emissiveSphere.EmissionColor = { 0.8f, 0.5f, 0.2f };
    emissiveSphere.EmissionPower = 1000.0f;

    Material& redSphere = m_Scene.Materials.emplace_back();
    redSphere.Albedo = { 0.9f, 0.15f, 0.25f };
    redSphere.Roughness = 0.85f;

    Material& whiteSphere = m_Scene.Materials.emplace_back();
    whiteSphere.Albedo = { 1.0f, 1.0f, 1.0f };
    whiteSphere.Roughness = 1.0f;

    m_Scene.Spheres.push_back(Sphere{ {0.0f, 0.0f, 0.0f}, 0.5f, 4, "White Sphere"});
    m_Scene.Spheres.push_back(Sphere{ {-1.5f, 0.0f, 0.8f}, 0.5f, 0, "Green Sphere" });
    m_Scene.Spheres.push_back(Sphere{ {1.5f, 0.0f, 0.8f}, 0.5f, 3, "Red Sphere" });
    m_Scene.Spheres.push_back(Sphere{ {0.0f, -21.5f, -0.8f}, 21.0f, 1, "Big Blue Sphere" });
    m_Scene.Spheres.push_back(Sphere{ {0.0f, 20.0f, -10.0f}, 0.8f, 2, "Emissive Sphere" });
}

void Stalwart::OnUpdate(float ts)
{
    if (m_Camera.OnUpdate(ts))
    {
        m_Renderer.ReserFrameIndex();
    }
}

void Stalwart::OnGUIRender(float ts)
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::Begin("Scene Hierachy");

    if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Add New Sphere"))
        {
            m_Scene.Spheres.push_back(Sphere{ {0.0f, 0.0f, 0.0f}, 0.5f, 0 });
        }

        ImGui::EndPopup();
    }

    for (int i = 0; i < m_Scene.Spheres.size(); i++)
    {
        ImGuiTreeNodeFlags flags = ((m_SelectedSphere == &m_Scene.Spheres[i]) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)&m_Scene.Spheres[i], flags, m_Scene.Spheres[i].Name.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectedSphere = &m_Scene.Spheres[i];
        }

        if (opened)
        {
            ImGui::TreePop();
        }
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_SelectedSphere != nullptr)
    {
        char* name = (char*)m_SelectedSphere->Name.c_str();
        int maxNameSize = 256;
        ImGui::InputText("Name", name, maxNameSize);
        ImGui::DragFloat3("Position", glm::value_ptr(m_SelectedSphere->Position), 0.1f);
        ImGui::DragFloat("Radius", &m_SelectedSphere->Radius, 0.1f);
        ImGui::DragInt("Material", &m_SelectedSphere->MaterialIndex, 1.0f, 0.0f, (int)m_Scene.Materials.size() - 1);
    }
    ImGui::End();

    ImGui::Begin("Material");
    if (m_SelectedSphere != nullptr)
    {
        int materialIndex = m_SelectedSphere->MaterialIndex;
        ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Scene.Materials[materialIndex].Albedo));
        ImGui::DragFloat("Roughness", &m_Scene.Materials[materialIndex].Roughness, 0.05f, 0.0f, 1.0f);
        ImGui::DragFloat("Metallic", &m_Scene.Materials[materialIndex].Metallic, 0.05f, 0.0f, 1.0f);
        ImGui::ColorEdit3("Emission Color", glm::value_ptr(m_Scene.Materials[materialIndex].EmissionColor));
        ImGui::DragFloat("Emission Power", &m_Scene.Materials[materialIndex].EmissionPower, 0.05f, 0.0f, FLT_MAX);
    }
    ImGui::End();

    ImGui::Begin("Scene Settings");
    ImGui::ColorEdit3("Sky color", glm::value_ptr(m_Scene.SkyColor));
    ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
    if (ImGui::Button("Reset"))
    {
        m_Renderer.ReserFrameIndex();
    }
    ImGui::Text("Metrics: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    m_ViewportWidth = ImGui::GetContentRegionAvail().x;
    m_ViewportHeight = ImGui::GetContentRegionAvail().y;

    Meek::Image* image = m_Renderer.GetFinalImage();

    if (image)
    {
        ImGui::Image(image->GetID(), { (float)image->GetWidth(), (float)image->GetHeight() }, { 0, 1}, { 1, 0});
    }

    ImGui::End();
    ImGui::PopStyleVar();

    Render();
}

void Stalwart::Render()
{
    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render(m_Scene, m_Camera);
}
