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
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);

private:
	uint32_t* m_ImageData = nullptr;
	Meek::Image* m_FinalImage = nullptr;
};