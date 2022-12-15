#version 450

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	sampler2D diffuseTex;
	sampler2D specularTex;
};

in vec3 vsPosition;
in vec3 vsColor;
in vec2 vsTexcoord;
in vec3 vsNormal;

out vec4 fs_color;

//uniforms
uniform Material material;
uniform vec3 lightPos0;
uniform vec3 cameraPos;

//functions
vec3 calculateAmbient(Material material)
{
	return material.ambient;
}

vec3 calculateDiffuse(Material material, vec3 vsPosition, vec3 vsNormal, vec3 lightPos0)
{
	vec3 posToLightDirVec = normalize(lightPos0 - vsPosition);
	float diffuse = clamp(dot(posToLightDirVec, vsNormal), 0, 1);
	vec3 diffuseFinal = material.diffuse * diffuse;

	return diffuseFinal;
}

vec3 calculateSpecular(Material material, vec3 vsPosition, vec3 vsNormal, vec3 lightPos0, vec3 cameraPos)
{
	vec3 lightToPosDirVec = normalize(lightPos0 - vsPosition);
	vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vsNormal)));
	vec3 posToViewDirVec = normalize(vsPosition - cameraPos);
	float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), 35);
	vec3 specularFinal = material.specular * specularConstant;

	return specularFinal;
}

void main()
{
	
	//ambient light
	vec3 ambientFinal = calculateAmbient(material);

	//diffuse light
	vec3 diffuseFinal = calculateDiffuse(material, vsPosition, vsNormal, lightPos0);

	//specular light
	vec3 specularFinal = calculateSpecular(material, vsPosition, vsNormal, lightPos0, cameraPos);

	//final light
	fs_color = vec4(vsColor, 1.f) *
	(
		vec4(ambientFinal, 1.f)
		+ vec4(diffuseFinal, 1.f) 
		+ vec4(specularFinal, 1.f)
	);
}