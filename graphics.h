#ifndef GRAPHICS_H
#define GRAPHICS_H

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <conio.h>
#include <sstream>
#include "constants.h"
#include "bot.h"

namespace gui {

	//Texture wrapper class
	class LTexture {
	public:
		LTexture(); //Initializes variables
		~LTexture(); //Deallocates memory
		bool loadFromFile(std::string path); //Loads image at specified path
#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
		//Creates image from font string
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif	
		void free(); //Deallocates texture
		void setColor(Uint8 red, Uint8 green, Uint8 blue); //Set color modulation
		void setBlendMode(SDL_BlendMode blending); //Set blending
		void setAlpha(Uint8 alpha); //Set alpha modulation
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE); //Renders texture at given point

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		SDL_Texture* mTexture; //The actual hardware texture

		//Image dimensions
		int mWidth;
		int mHeight;
	};

	class LWindow {
	public:
		//Intializes internals
		LWindow();

		//Creates window
		bool init();

		//Creates renderer from internal window
		SDL_Renderer* createRenderer();

		//Handles window events
		void handleEvent(SDL_Event& e);

		//Deallocates internals
		void free();

		//Window dimensions
		int getWidth();
		int getHeight();

		//Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();

		//Window data
		SDL_Window* mWindow;
	private:
		//Window dimensions
		int mWidth;
		int mHeight;

		//Window focus
		bool mMouseFocus;
		bool mKeyboardFocus;
		bool mMinimized;
	};

	void checkEvents();
	void draw();
	bool initialize();
	void close();
	void updateStats();

} /* ::gui */

#endif /* GRAPHICS_H */