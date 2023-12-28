#pragma once

#include "Core/Application.h"

class Stalwart final : public Meek::Application
{
    using Meek::Application::Application;

    virtual void OnAttach() override;
    virtual void OnGUIRender() override;

    void Render();

    uint32_t m_ViewportWidth = 0;
    uint32_t m_ViewportHeight = 0;

    uint32_t* m_ImageData = nullptr;
    Meek::Image* m_Image = nullptr;
};