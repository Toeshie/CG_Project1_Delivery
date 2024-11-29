#pragma once

#include <SDL.h>
#include <glad/glad.h>
#include <string>

class WindowManager {
public:
	WindowManager(const std::string& title, int width, int height);
	~WindowManager();

	bool initialize();
	void handleEvents(bool& running);
	void swapBuffers() const;

	SDL_Window* getWindow() const { return window; }

private:
	std::string title;
	int width;
	int height;
	SDL_Window* window;
	SDL_GLContext glContext;
};


