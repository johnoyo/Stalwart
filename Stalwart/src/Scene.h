#pragma once

#include <glm/glm.hpp>

#include <vector>

struct Material
{
	glm::vec3 Albedo{1.0f};
	float Roughness = 1.0f;
	float Metallic = 0.0f;

	glm::vec3 EmissionColor;
	float EmissionPower;
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;
	int MaterialIndex = 0;
};

struct Scene
{
	glm::vec3 SkyColor{0.6f, 0.7f, 0.9f};

	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};