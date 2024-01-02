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
    m_ActiveCamera = &camera;
    m_ActiveScene = &scene;

    if (m_FrameIndex == 1)
    {
        memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
    }

    for (int y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (int x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec4 color = PerPixel(x, y);

            m_AccumulationData[x + y * (uint32_t)m_FinalImage->GetWidth()] += color;

            glm::vec4 accumulatedColor = m_AccumulationData[x + y * (uint32_t)m_FinalImage->GetWidth()];
            accumulatedColor /= (float)m_FrameIndex;

            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * (uint32_t)m_FinalImage->GetWidth()] = Utilities::ConvertToRGBA(accumulatedColor);
        }
    }

    m_FinalImage->Invalidate(m_ImageData);

    if (m_Settings.Accumulate)
    {
        m_FrameIndex++;
    }
    else
    {
        m_FrameIndex = 1;
    }
}

Meek::Image* Renderer::GetFinalImage()
{
    return m_FinalImage;
}

void Renderer::ReserFrameIndex()
{
    m_FrameIndex = 1;
}

Renderer::Settings& Renderer::GetSettings()
{
    return m_Settings;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * (uint32_t)m_FinalImage->GetWidth()];

    glm::vec3 color(0.0f);
    float multiplier = 1.0f;

    int bounces = 5;
    for (int i = 0; i < bounces; i++)
    {
        Renderer::HitPayload payload = TraceRay(ray);

        if (payload.HitDistance < 0.0f)
        {
            glm::vec3 skyColor(0.6f, 0.7f, 0.9f);
            color += skyColor * multiplier;
            break;
        }

        glm::vec3 lightDirection = glm::normalize(glm::vec3(-1, -1, -1));

        float d = (glm::max)(glm::dot(payload.WorldNormal, -lightDirection), 0.0f); // == cos(theta)

        const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
        const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

        glm::vec3 sphereColor = material.Albedo;
        sphereColor *= d;
        color += sphereColor * multiplier;

        multiplier *= 0.5f;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + material.Roughness * Meek::Random::Vec3(-0.5f, 0.5f));
    }

    return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
    // (bx^2 + by^2 + bz^2)t^2 + 2*(ax*bx + ay*by + az*bz)*t + (ax^2 + ay^2 + az^2 - r^2) = 0
    // where
    // a = ray origin
    // b = ray direction
    // r = radious
    // t = hit distance

    uint32_t closestSphere = (std::numeric_limits<uint32_t>::max)();
    float closestDistance = (std::numeric_limits<float>::max)();

    for (int i = 0; i < m_ActiveScene->Spheres.size(); i++)
    {
        const Sphere& sphere = m_ActiveScene->Spheres[i];

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

        if (closestT > 0.0f && closestT < closestDistance)
        {
            closestDistance = closestT;
            closestSphere = i;
        }
    }

    if (closestSphere == (std::numeric_limits<uint32_t>::max)())
    {
        return Miss(ray);
    }

    return ClosestHit(ray, closestDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex)
{
    HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

    glm::vec3 origin = ray.Origin - closestSphere.Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);

    payload.WorldPosition += closestSphere.Position;

    return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
    Renderer::HitPayload payload;
    payload.HitDistance = -1.0f;
    return payload;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage == nullptr || m_FinalImage->GetWidth() != width || m_FinalImage->GetHeight() != height)
    {
        delete m_FinalImage;
        m_FinalImage = new Meek::Image(width, height);

        delete[] m_ImageData;
        m_ImageData = new uint32_t[width * height];

        delete[] m_AccumulationData;
        m_AccumulationData = new glm::vec4[width * height];
    }
}
