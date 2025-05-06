#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TextureCoordinate;
layout (location = 3) in vec3 Tangent;

uniform mat4 modelMatrix;
uniform mat4 transform;

out VERTEX_OUT
{
	vec3 frag_pos; 
	vec2 texture_coordinate;
	vec3 vertex_normal;

	mat3 TBN;
} v_out;

void main()
{
	// Fragment Position.
	v_out.frag_pos = (modelMatrix * vec4(VertexPosition, 1.0f)).xyz;
	v_out.texture_coordinate = TextureCoordinate;

	
	// Calculate the TBN Matrix.
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	v_out.vertex_normal = normalize(normalMatrix * VertexNormal);
	vec3 tangent = normalize(normalMatrix * Tangent);
	tangent = normalize(tangent - dot(tangent, v_out.vertex_normal) * v_out.vertex_normal);
	vec3 biTangent = cross(v_out.vertex_normal, tangent);

	v_out.TBN = transpose(mat3(tangent, biTangent, v_out.vertex_normal));


	// Output Position.
	gl_Position = transform * vec4(VertexPosition, 1.0);
}