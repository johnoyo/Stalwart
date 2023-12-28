#include "Stalwart.h"

void Stalwart::OnAttach()
{
    Meek::Random::Init();

    m_Camera = Camera(45.0f, 0.1f, 100.0f);
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
    }    

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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
    std::cout << "Rendering\n";

    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render(m_Camera);
}
