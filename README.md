# GP2-Coursework
 A repository for the work done on our Games Programming 2 module's Coursework


## Controls
W, A, S, D, LCtrl, Space - Camera Movement
Mouse Input - Camera Look
Q - Increase Day Speed
Esc - Toggle Mouse Input


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
- Matrices:
	- MVP Matrix:			transform
	- Model Matrix:			modelMatrix
	- View Matrix:			viewMatrix
	- Projection Matrix:	projectionMatrix
- Textures (Where 'N' represents any positive number):
	- Diffuse:	texture_diffuseN
	- Specular:	texture_specularN
	- Normal:	texture_normalN
	- Height:	texture_heightN


## Asset Credits
Bench Model:
- SIV Assets - "Stylized Wooden Benmch PBR" 'https://assetstore.unity.com/packages/3d/props/stylized-wooden-bench-pbr-267777'
Terrain Model:
- Render Knight - "Fantasy Skybox Free" 'https://assetstore.unity.com/packages/2d/textures-materials/sky/fantasy-skybox-free-18353'
- (Converted to a Mesh using) zwcloud - "Mesh Terrain Editor Free" 'https://assetstore.unity.com/packages/tools/terrain/mesh-terrain-editor-free-67758'
Low Poly Fir Tree Model:
- BOXOPHOBIC - "FREE Skybox Extended Shader" 'https://assetstore.unity.com/packages/vfx/shaders/free-skybox-extended-shader-107400'

Skybox Images:
- (Night & Day) BOXOPHOBIC - "Polyverse Skies | Low Poly Skybox Shaders" 'https://assetstore.unity.com/packages/vfx/shaders/polyverse-skies-low-poly-skybox-shaders-104017'