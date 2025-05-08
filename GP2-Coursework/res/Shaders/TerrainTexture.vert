#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TextureCoordinates;
layout (location = 3) in vec3 Tangent;

out VERTEX_OUT
{
	vec3 frag_pos;
	vec2 texture_coordinates;

	mat3 TBN;
	vec3 tangent_view_pos;
	vec3 tangent_frag_pos;
} v_out;

uniform mat4 modelMatrix;
uniform mat4 transform;

uniform vec3 view_pos;


void main()
{
	// Fragment Position.
	v_out.frag_pos = vec3(modelMatrix * vec4(VertexPosition, 1.0f));
	v_out.texture_coordinates = TextureCoordinates;

	// Calculate the TBN Matrix.
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 normal = normalize(normalMatrix * VertexNormal);
	vec3 tangent = normalize(normalMatrix * Tangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 biTangent = cross(normal, tangent);

	v_out.TBN = transpose(mat3(tangent, biTangent, normal));
	v_out.tangent_view_pos = v_out.TBN * view_pos;
	v_out.tangent_frag_pos = v_out.TBN * v_out.frag_pos;


	// Output Position.
	gl_Position = transform * vec4(VertexPosition, 1.0);
}