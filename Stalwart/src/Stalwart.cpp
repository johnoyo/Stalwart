#include "Stalwart.h"

#include "glm/gtc/type_ptr.hpp"

void Stalwart::OnAttach()
{
    Meek::Random::Init();

    m_Camera = Camera(45.0f, 0.1f, 100.0f);

    Material& greenSphere = m_Scene.Materials.emplace_back();
    greenSphere.Albedo = { 0.35f, 1.0f, 0.0f, 1.0f };
    greenSphere.Roughness = 0.75f;

    Material& blueSphere = m_Scene.Materials.emplace_back();
    blueSphere.Albedo = { 0.0f, 0.2f, 1.0f, 1.0f };
    blueSphere.Roughness = 0.05f;

    m_Scene.Spheres.push_back(Sphere{ {0.0f, 0.0f,0.0f}, 0.5f, 0 });
    m_Scene.Spheres.push_back(Sphere{ {0.0f, -21.5f, -0.8f}, 21.0f, 1 });
}

void Stalwart::OnUpdate(float ts)
{
    m_Camera.OnUpdate(ts);
}

void Stalwart::OnGUIRender(float ts)
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::Begin("Settings");

    if (ImGui::Button("Render"))
    {
        Render();
    }    

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::Begin("Scene");
    for (int i = 0; i < m_Scene.Spheres.size(); i++)
    {
        ImGui::PushID(i);

        ImGui::DragFloat3("Position", glm::value_ptr(m_Scene.Spheres[i].Position), 0.1f);
        ImGui::DragFloat("Radius", &m_Scene.Spheres[i].Radius, 0.1f);
        ImGui::DragInt("Material", &m_Scene.Spheres[i].MaterialIndex, 1.0f, 0.0f, (int)m_Scene.Materials.size() - 1);

        ImGui::Separator();

        ImGui::PopID();
    }

    for (int i = 0; i < m_Scene.Materials.size(); i++)
    {
        ImGui::PushID(i);

        ImGui::ColorEdit4("Albedo", glm::value_ptr(m_Scene.Materials[i].Albedo));
        ImGui::DragFloat("Roughness", &m_Scene.Materials[i].Roughness, 0.05f, 0.0f, 1.0f);
        ImGui::DragFloat("Metallic", &m_Scene.Materials[i].Metallic, 0.05f, 0.0f, 1.0f);

        ImGui::Separator();

        ImGui::PopID();
    }

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
