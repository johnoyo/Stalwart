#pragma once

#include "Core\Image.h"
#include "Core\Random.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include "glm/glm.hpp"

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera);

	Meek::Image* GetFinalImage();

private:
	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;
		uint32_t ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	HitPayload TraceRay(const Ray& ray);
	HitPayload ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex);
	HitPayload Miss(const Ray& ray);

private:
	uint32_t* m_ImageData = nullptr;
	Meek::Image* m_FinalImage = nullptr;

	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;
};