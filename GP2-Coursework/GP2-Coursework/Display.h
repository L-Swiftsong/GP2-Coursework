#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <string>


class Display
{
public:
	Display();
	~Display();
	void SwapBuffer();
	void ClearDisplay(float r, float g, float b, float a);

	float get_screen_width();
	float get_screen_height();

private:
	void ReturnError(std::string error_string);
	void InitDisplay();
	
	SDL_GLContext gl_context_; //global variable to hold the context
	SDL_Window* sdl_window_; //holds pointer to out window
	float screen_width_;
	float screen_height_;
};

