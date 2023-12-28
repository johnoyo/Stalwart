#pragma once

#include "Core\Image.h"
#include "Core\Random.h"

#include "Camera.h"
#include "Ray.h"

#include "glm/glm.hpp"

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera);

	Meek::Image* GetFinalImage();

private:
	glm::vec4 TraceRay(const Ray& ray);

private:
	uint32_t* m_ImageData = nullptr;
	Meek::Image* m_FinalImage = nullptr;
};