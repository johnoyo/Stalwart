#pragma once

#include "Core/Application.h"
#include "Renderer.h"
#include "Camera.h"

class Stalwart final : public Meek::Application
{
    using Meek::Application::Application;

    virtual void OnAttach() override;
    virtual void OnUpdate(float ts) override;
    virtual void OnGUIRender(float ts) override;

    void Render();

    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;

    uint32_t m_ViewportWidth = 0;
    uint32_t m_ViewportHeight = 0;

    Sphere* m_SelectedSphere = nullptr;
};