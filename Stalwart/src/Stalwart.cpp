#include "Stalwart.h"

void Stalwart::OnAttach()
{
}

void Stalwart::OnGUIRender()
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::Begin("Settings");

    if (ImGui::Button("Render"))
    {
        Render();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::Begin("Viewport");

    m_ViewportWidth = ImGui::GetContentRegionAvail().x;
    m_ViewportHeight = ImGui::GetContentRegionAvail().y;

    if (m_Image)
    {
        ImGui::Image(m_Image->GetID(), { (float)m_Image->GetWidth(), (float)m_Image->GetHeight() });
    }

    ImGui::End();
}

void Stalwart::Render()
{
    std::cout << "Rendering\n";

    if (m_Image == nullptr || m_Image->GetWidth() != m_ViewportWidth || m_Image->GetHeight() != m_ViewportHeight)
    {
        m_Image = new Meek::Image(m_ViewportWidth, m_ViewportHeight);

        delete[] m_ImageData;

        m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
    }

    for (int i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
    {
        m_ImageData[i] = 0xffff00ff;
    }

    m_Image->Invalidate(m_ImageData);
}
