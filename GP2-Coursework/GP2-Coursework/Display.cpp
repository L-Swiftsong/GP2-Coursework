#include "Display.h"


Display::Display()
{
	// Initialise to generate null access violation for debugging. 
	sdl_window_ = nullptr;
	gl_context_ = nullptr;

	screen_width_ = 1024.0f;
	screen_height_ = 768.0f;

	InitDisplay();
}

Display::~Display()
{
	SDL_GL_DeleteContext(gl_context_); // delete context
	SDL_DestroyWindow(sdl_window_); // detete window (make sure to delete the context and the window in the opposite order of creation in initDisplay())
	SDL_Quit();
}

void Display::ReturnError(std::string error_string)
{
	std::cout << error_string << std::endl;
	std::cout << "press any  key to quit...";
	int in;
	std::cin >> in;
	SDL_Quit();
}

void Display::SwapBuffer()
{
	// Swap buffers.
	SDL_GL_SwapWindow(sdl_window_);
}

void Display::ClearDisplay(float r, float g, float b, float a)
{
	// (Temp).
	glViewport(0, 0, screen_width_, screen_height_);

	// Clear colour and depth buffer - set colour to colour defined in glClearColor.
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::InitDisplay()
{
	// Initalise everything.
	SDL_Init(SDL_INIT_EVERYTHING);

	// Min no of bits used to diplay colour.
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	// Set up z-buffer.
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	// Set up double buffer.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Create window.
	sdl_window_ = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)screen_width_, (int)screen_height_, SDL_WINDOW_OPENGL);

	if (sdl_window_ == nullptr)
	{
		ReturnError("window failed to create");
	}

	// Initialise the context.
	gl_context_ = SDL_GL_CreateContext(sdl_window_);

	if (gl_context_ == nullptr)
	{
		ReturnError("SDL_GL context failed to create");
	}

	// Initialise GLEW.
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		ReturnError("GLEW failed to initialise");
	}

	// Initialise our Input & Mouse Capturing.
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Enable z-buffering.
	glEnable(GL_DEPTH_TEST);
	// Dont draw faces that are not pointing at the camera.
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
}


float Display::get_screen_width() { return screen_width_; }
float Display::get_screen_height() { return screen_height_; }