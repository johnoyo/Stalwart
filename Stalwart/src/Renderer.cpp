#include "Renderer.h"

void Renderer::Render()
{
    for (int y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (int x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { x / m_FinalImage->GetWidth(), y / m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; // -1 to 1
            m_ImageData[x + y * (uint32_t)m_FinalImage->GetWidth()] = PerPixel(coord);
        }
    }

    m_FinalImage->Invalidate(m_ImageData);
}

Meek::Image* Renderer::GetFinalImage()
{
    return m_FinalImage;
}

uint32_t Renderer::PerPixel(glm::vec2& coord)
{
    glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
    glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
    float radius = 0.5f;

    // rayDirection = glm::normalize(rayDirection);

    // (bx^2 + by^2 + bz^2)t^2 + 2*(ax*bx + ay*by + az*bz)*t + (ax^2 + ay^2 + az^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radious
    // t = hit distance

    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(rayOrigin, rayDirection);
    float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

    // Discriminant
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant >= 0.0f)
    {
        return 0xffff00ff;
    }

    return 0xff000000;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage == nullptr || m_FinalImage->GetWidth() != width || m_FinalImage->GetHeight() != height)
    {
        delete m_FinalImage;

        m_FinalImage = new Meek::Image(width, height);

        delete[] m_ImageData;

        m_ImageData = new uint32_t[width * height];
    }
}
