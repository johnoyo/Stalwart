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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    Ray ray;
    ray.Origin = camera.GetPosition();

    for (int y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (int x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { x / m_FinalImage->GetWidth(), y / m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; // -1 to 1

            ray.Direction = camera.GetRayDirections()[x + y * (uint32_t)m_FinalImage->GetWidth()];

            glm::vec4 color = TraceRay(scene, ray);

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

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
    // (bx^2 + by^2 + bz^2)t^2 + 2*(ax*bx + ay*by + az*bz)*t + (ax^2 + ay^2 + az^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radious
    // t = hit distance

    if (scene.Spheres.size() == 0)
    {
        return glm::vec4(0, 0, 0, 1);
    }

    const Sphere* closestSphere = nullptr;
    float closestDistance = (std::numeric_limits<float>::max)();

    for (const Sphere& sphere : scene.Spheres)
    {
        glm::vec3 origin = ray.Origin - sphere.Position;

        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        // Discriminant
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f)
        {
            continue;
        }

        // (-b +- sqrt(discriminant)) / 2a
        // float t0 = (-b + glm::sqrt(discriminant)) / (2 * a);
        float closestT = (-b - glm::sqrt(discriminant)) / (2 * a);

        if (closestT < closestDistance)
        {
            closestDistance = closestT;
            closestSphere = &sphere;
        }
    }

    if (closestSphere == nullptr)
    {
        return glm::vec4(0, 0, 0, 1);
    }

    glm::vec3 origin = ray.Origin - closestSphere->Position;
    glm::vec3 hitPoint = origin + ray.Direction * closestDistance;
    glm::vec3 normal = glm::normalize(hitPoint);

    glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));

    float d = glm::max(glm::dot(normal, -lightDirection), 0.0f); // == cos(theta)

    glm::vec3 sphereColor = closestSphere->Albedo;
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
