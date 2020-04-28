#pragma once
#ifndef WINDOW_H
#define WINDOW_H
#include "WindowEventListener.h"

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include "GaiaExports.h"

#include <SDL2/SDL.h>

typedef Ogre::Viewport Viewport;
typedef Ogre::RenderWindow RenderWindow;

class Window : public WindowEventListener
{
public:
	Window(Ogre::Root* root, std::string windowTitle);
	~Window();

	Viewport* addViewport(Ogre::Camera* cam);
	RenderWindow* getRenderWindow();

	void removeAllViewports();

	void setFullscreen(bool fullscreen);
	void setFSAA(unsigned int fsaa);

	void move(int x, int y);
	void resize(unsigned int width, unsigned int height);
	bool isClosed();
	void close();

	void setWindowMinArea(int width, int height);
	void setWindowResizable(bool resize);

private:
	void moved(unsigned int x, unsigned int y);
	void resized(unsigned int width, unsigned int height);
	

private:
	Ogre::Root* root;

	SDL_Window* sdlWindow;
	RenderWindow* window;

	bool isResizable;

	int xPosition;
	int yPosition;

	unsigned int width;
	unsigned int height;

	bool closed;
};

#endif
