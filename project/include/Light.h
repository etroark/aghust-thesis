#pragma once

#include "libs.h"

class Light
{
protected:

	float intensity;
	glm::vec3 color;

public:

	Light(float intensity, glm::vec3 color)
	{
		this->intensity = intensity;
		this->color = color;
	}
};

class PointLight : public Light
{
protected:

	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

public:

	PointLight
	(glm::vec3 position,
		float intensity = 1.f,
		glm::vec3 color = glm::vec3(1.f),
		float constant = 1.f,
		float linear = 0.05f,
		float quadratic = .0075f
	) : Light(intensity, color)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	~PointLight() {}

	void setPosition(const glm::vec3 position)
	{
		this->position = position;
	}
};