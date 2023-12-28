#include "Renderer.h"

namespace Utilities
{
    static uint32_t ConvertToRGBA(const glm::vec4& color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        return (a << 24) | (b << 16) | (g << 8) | r;
    }
}

void Renderer::Render()
{
    for (int y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (int x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { x / m_FinalImage->GetWidth(), y / m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; // -1 to 1
            glm::vec4 color = PerPixel(coord);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * (uint32_t)m_FinalImage->GetWidth()] = Utilities::ConvertToRGBA(color);
        }
    }

    m_FinalImage->Invalidate(m_ImageData);
}

Meek::Image* Renderer::GetFinalImage()
{
    return m_FinalImage;
}

glm::vec4 Renderer::PerPixel(glm::vec2& coord)
{
    glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
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

    if (discriminant < 0.0f)
    {
        return glm::vec4(0, 0, 0, 1);
    }

    // (-b +- sqrt(discriminant)) / 2a
    float t0 = (-b + glm::sqrt(discriminant)) / (2 * a);
    float closestT = (-b - glm::sqrt(discriminant)) / (2 * a);

    glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
    glm::vec3 normal = glm::normalize(hitPoint);

    glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));

    float d = glm::max(glm::dot(normal, -lightDirection), 0.0f); // == cos(theta)

    glm::vec3 sphereColor(1, 0, 1);
    sphereColor *= d;

    return glm::vec4(sphereColor, 1.0f);
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
