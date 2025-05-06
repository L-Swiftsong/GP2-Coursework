#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <iostream>
#include <string>


// 1024x768
#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720

class Display
{
public:
	Display();
	~Display();
	void SwapBuffer();
	void ClearDisplay(float r, float g, float b, float a);


	void WindowSizeChanged();

	int get_screen_width() const;
	int get_screen_height() const;

private:
	void ReturnError(std::string error_string);
	void InitDisplay();
	
	SDL_GLContext gl_context_; //global variable to hold the context
	SDL_Window* sdl_window_; //holds pointer to out window
	int screen_width_;
	int screen_height_;
};

