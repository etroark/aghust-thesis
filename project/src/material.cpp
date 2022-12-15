#include "../include/material.h"

void Material::sendToShader(Shader& program)
{
	//those are in the glsl files
	program.setVec3f(_ambientLight, "material.ambient");
	program.setVec3f(_diffuseLight, "material.diffuse");
	program.setVec3f(_specularLight, "material.specular");
	//program.set1i(_diffuseTex, "material.diffuseTex");
	//program.set1i(_specularTex, "material.specularTex");
}