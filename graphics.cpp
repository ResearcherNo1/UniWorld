#include "graphics.h"

extern SDL_Renderer* gRenderer;
extern gui::LWindow gWindow;
extern SDL_Rect gSpriteClips[2];
extern gui::LTexture gSpriteBot;
extern gui::LTexture gSpriteOrganic;
extern bool work;
extern bool pause;
extern bool paintMode;
extern uint_fast64_t lifeCount;
extern uint_fast64_t EnterlifeCount;
extern int season;

extern long long world[WORLD_WIDTH][((unsigned long long)WORLD_HEIGHT + 2)];
extern std::pmr::vector<bot> bots;

gui::LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

gui::LTexture::~LTexture() {
	//Deallocate
	free();
}

bool gui::LTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool gui::LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void gui::LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void gui::LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void gui::LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void gui::LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void gui::LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int  gui::LTexture::getWidth() {
	return mWidth;
}
	  
int  gui::LTexture::getHeight() {
	return mHeight;
}



gui::LWindow::LWindow() {
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

bool gui::LWindow::init() {
	//Create window
	mWindow = SDL_CreateWindow("UniWorld", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}

	return mWindow != NULL;
}

SDL_Renderer* gui::LWindow::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void gui::LWindow::handleEvent(SDL_Event& e) {

		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event) {

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}
}

void gui::LWindow::rename(std::string _new) {
	SDL_SetWindowTitle(mWindow, _new.c_str());
}

void gui::LWindow::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int gui::LWindow::getWidth() {
	return mWidth;
}

int gui::LWindow::getHeight() {
	return mHeight;
}

bool gui::LWindow::hasMouseFocus() {
	return mMouseFocus;
}

bool gui::LWindow::hasKeyboardFocus() {
	return mKeyboardFocus;
}




void gui::checkEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		//Обработка закрытия
		if (e.type == SDL_QUIT)
			work = false;
		//Обработка событий окна
		else if (e.type == SDL_WINDOWEVENT)
			gWindow.handleEvent(e);
		//Обработка нажатий кнопок
		else if (e.type == SDL_KEYDOWN) {
			//Обработка выхода через Esc
			if (e.key.keysym.sym == SDLK_ESCAPE)
				work = false;
			//Обработка паузы ("F1")
			else if (e.key.keysym.sym == SDLK_F1) {
				pause = !pause;
				if (pause)
					SDL_SetWindowTitle(gWindow.mWindow, "UniWorld - Paused");
				else
					SDL_SetWindowTitle(gWindow.mWindow, "UniWorld");
			}
			//Обработка смены режима ("F2")
			else if (e.key.keysym.sym == SDLK_F2) {
				paintMode = !paintMode;
				if (paintMode)
					SDL_SetRenderDrawColor(gRenderer, 0xFB, 0xF3, 0xCD, 0xFF);
				else
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				
				gui::draw();
			}
			//Обработка просчёта шага ("Enter")
			else if (e.key.keysym.sym == SDLK_KP_ENTER)
				if (pause)
					EnterlifeCount = lifeCount + 1;

		}
	}
}

void gui::draw() {
	SDL_RenderClear(gRenderer);

	for (size_t i = 0; i < bots.size(); i++) {
		//Отрисовка живого бота
		if (bots[i].condition == alive) {
			if (paintMode) //Если включён стандартный режим отрисовки
				gSpriteBot.setColor(bots[i].red, bots[i].green, bots[i].blue);
			else
				gSpriteBot.setColor(0xFF, 255 - (bots[i].energy / 4), 0x00);

			gSpriteBot.render((bots[i].coorX * 4), ((bots[i].coorY - 1) * 4), &gSpriteClips[0]);
		}
		//Отрисовка органики
		else if (bots[i].condition < organic_sink) {
			if (paintMode)
				gSpriteOrganic.setColor(0xFF, 0xFF, 0xFF);
			else
				gSpriteOrganic.setColor(0xE4, 0x7F, 0xF6);

			gSpriteOrganic.render((bots[i].coorX * 4), ((bots[i].coorY - 1) * 4), &gSpriteClips[0]);
		}
	}

	//Update screen
	SDL_RenderPresent(gRenderer);

	if (pause)
		gWindow.rename("UniWorld - Paused");
	else
		gWindow.rename("UniWorld");
}

bool gui::initialize() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}
		//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) 
		printf("Warning: Linear texture filtering not enabled!");

	//Create window
	if (!gWindow.init()) {
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Create renderer for window
	gRenderer = gWindow.createRenderer();
	if (gRenderer == NULL) {
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//Initialize renderer color
	SDL_SetRenderDrawColor(gRenderer, 0xFB, 0xF3, 0xCD, 0xFF);
	SDL_RenderClear(gRenderer);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
		return false;
	}


	//Load textures
	if (!(gSpriteBot.loadFromFile("textures\\bot.png"))) {
		printf("Failed to load bot sprite sheet texture!\n");
		return false;
	}
	if (!(gSpriteOrganic.loadFromFile("textures\\organic.png"))) {
		printf("Failed to load organic sprite sheet texture!\n");
		return false;
	}

	//Sets sprites
	for (char i = 0; i < 2; i++) {
		gSpriteClips[i].x = gSpriteClips[i].y = 0;
		gSpriteClips[i].h = gSpriteClips[i].w = 4;
	}

	return true;
}

void gui::close() {
	//Free loaded images
	gSpriteBot.free();
	gSpriteOrganic.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void gui::updateStats() {
	lifeCount++;
	system("cls");
	printf("Generation step: %lli\n", lifeCount);
	printf("Count of bots: %lli\n", bots.size());

	if (!(lifeCount % 1000)) {
		if (season > 9)
			season--;
		else
			season = 11;
	}
}
