#version 400
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadow_matrices[6];

out vec4 FragPos;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // gl_Layer is a build-in variable which specifies what face we're rendering to.
		for(int i = 0; i < 3; ++i) // Repeat once for each triangle vertex.
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadow_matrices[face] * FragPos;
			EmitVertex();
		}

		EndPrimitive();
	}
}