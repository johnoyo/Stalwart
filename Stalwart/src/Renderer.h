#pragma once

#include "Core\Image.h"
#include "Core\Random.h"

#include "glm/glm.hpp"

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render();

	Meek::Image* GetFinalImage();

private:
	glm::vec4 PerPixel(glm::vec2& coord);

private:
	uint32_t* m_ImageData = nullptr;
	Meek::Image* m_FinalImage = nullptr;
};