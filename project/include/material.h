#pragma once

#include "libs.h"

#include "shader.h"

/**
 * material class
 */
class Material
{
	glm::vec3 _ambientLight;	/**< ambient light strength*/
	glm::vec3 _diffuseLight;	/**< diffuse light strength*/
	glm::vec3 _specularLight;	/**< specular light strength*/
	//GLint _diffuseTex;
	//GLint _specularTex;

public:

	/**
	 * class constructor
	 * @param ambient ambient light strength
	 * @param diffuse diffuse light strength
	 * @param specular specular light strength
	 */
	inline Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		_ambientLight(ambient),
		_diffuseLight(diffuse),
		_specularLight(specular)
		//_diffuseTex(diffuseTex),
		//_specularTex(specularTex)
	{}

	/**
	 * sends material properties to the fragment shader
	 * @param program reference to the shader program
	 */
	void sendToShader(Shader& program);
};