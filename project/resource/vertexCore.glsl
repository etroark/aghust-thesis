#version 450

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

out vec3 vsPosition;
out vec3 vsColor;
out vec2 vsTexcoord;
out vec3 vsNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
	vsPosition = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
	vsColor = vertex_color;
	vsTexcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);
	vsNormal = mat3(ModelMatrix) * vertex_normal;

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}