#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TextureCoordinate;

out VERTEX_OUT
{
	vec3 frag_pos;
	vec3 normal;
	vec2 texture_coordinate;
	vec4 frag_pos_light_space;
} v_out;

uniform mat4 light_space_matrix;
uniform mat4 modelMatrix;
uniform mat4 transform;


void main()
{
	v_out.frag_pos = vec3(modelMatrix * vec4(VertexPosition, 1.0f));
	v_out.normal = transpose(inverse(mat3(modelMatrix))) * VertexNormal;
	v_out.texture_coordinate = TextureCoordinate;
	v_out.frag_pos_light_space = light_space_matrix * vec4(v_out.frag_pos, 1.0f);

	gl_Position = transform * vec4(VertexPosition, 1.0f);
}