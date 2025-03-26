# GP2-Coursework
 A repository for the work done on our Games Programming 2 module's Coursework


## Naming Conventions
C++ Style Link: https://google.github.io/styleguide/cppguide.html

Quick Notes (C++):
- Variables: Snake Case
	- Private (Classes Only): '_' suffix
	- Constants: 'k' prefix
- Enums:
	- Name enums like constants ('k' prefix), not macros (All caps)
- Functions:
	- Pascal Case (Start with caps, first letter of words is capitalised)
	- Accessors & Mutators (Getters & Setters): Named like variables (E.g. int count -> set_count(int count))


Shader Notes:
- Textures (Where 'N' represents any positive number):
	- Diffuse:	texture_diffuseN
	- Specular:	texture_specularN
	- Normal:	texture_normalN
	- Height:	texture_heightN