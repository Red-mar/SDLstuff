#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "stdio.h"
#include <string>
#include <cmath>

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 480

const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 100;
const int TOTAL_BUTTONS = 4;

SDL_Window *screen = NULL;
SDL_Renderer * sdlRenderer = NULL;
TTF_Font *gFont = NULL;

enum LButtonSprite{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};

class LTexture
{
    public:
        LTexture();
        ~LTexture();

        bool loadFromFile( std::string path);

        #ifdef _SDL_TTF_H
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor);
        #endif

        void free();

        void setColor( Uint8 red, Uint8 green, Uint8 blue);

        void setBlendMode( SDL_BlendMode blending);

        void setAlpha( Uint8 alpha);

        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        int getWidth();
        int getHeight();

    private:
        SDL_Texture* mTexture;
        int mWidth;
        int mHeight;
};

LTexture::LTexture()
{
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromFile( std::string path)
{
    free();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf_s("Unable to load image at %s, Error: %s", path.c_str(), IMG_GetError());
    } else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    }
    newTexture = SDL_CreateTextureFromSurface(sdlRenderer, loadedSurface);
    if (newTexture == NULL)
    {
        printf_s("Unable to create texture from %s, Error: %s\n", path.c_str(), SDL_GetError());
    } else {
        mWidth = loadedSurface->w;
        mHeight = loadedSurface->h;
    }

    SDL_FreeSurface( loadedSurface );

    mTexture = newTexture;
    return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor){
    free();
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor);
    if (textSurface == NULL)
    {
        printf_s("Unable to render text surface. Error: %s\n", TTF_GetError());
    } else {
        mTexture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);
        if (mTexture == NULL)
        {
            printf_s("Unable to create texture from text, Error %s\n", SDL_GetError());
        } else {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        SDL_FreeSurface(textSurface);
    }

    return mTexture != NULL;
}
#endif

void LTexture::free(){
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(sdlRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getHeight(){
    return mHeight;
}

int LTexture::getWidth(){
    return mWidth;
}

class LButton
{
    public:
        LButton();

        void setPosition(int x, int y);

        void handleEvent(SDL_Event* e);

        void render();

    private:
        SDL_Point mPosition;
        LButtonSprite mCurrentSprite;
};

LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;
    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y){
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent( SDL_Event* e)
{
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {
        int x, y;
        SDL_GetMouseState( &x, &y);

        bool inside = true;

        if (x < mPosition.x)
        {
            inside = false;
        }
        else if(x > mPosition.x + BUTTON_WIDTH)
        {
            inside = false;
        }
        else if(y < mPosition.y)
        {
            inside = false;
        }
        else if (y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }

        if (!inside)
        {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        else {
            switch (e->type)
            {
            case SDL_MOUSEMOTION:
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
            break;

            case SDL_MOUSEBUTTONDOWN:
            mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
            break;

            case SDL_MOUSEBUTTONUP:
            mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
            break;
            }
        }
    }
}

const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LButton gButtons[BUTTON_SPRITE_TOTAL];
LTexture gTexture;
LTexture gBackgroundTexture;
LTexture gTextTexture;

void LButton::render(){
    gTexture.render(mPosition.x, mPosition.y, &gSpriteClips[ mCurrentSprite]);
}



bool init(){
    SDL_Init(SDL_INIT_EVERYTHING);

    screen = SDL_CreateWindow("My first window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_SWSURFACE);
    if (screen == NULL)
    {
        printf_s("Window could not be created. error: %s\n", SDL_GetError());
        return false;
    }
    sdlRenderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlRenderer == NULL)
    {
        printf_s("Renderer could not be created. error: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf_s("SLD image could not be initialized. error:%s\n", IMG_GetError());
        return false;
    }
    if (TTF_Init() == -1)
    {
        printf_s("TTF could not initialize, Error %s\n",TTF_GetError());
        return false;
    }
    return true;
}

bool loadMedia(){
    bool success = true;

    gFont = TTF_OpenFont("OpenSans-Regular.ttf", 72);
    if (gFont == NULL)
    {
        printf_s("Failed to load font, Error %s\n", TTF_GetError());
        success = false;
    } else {
        SDL_Color textColor = {0,0,0};
        if (!gTextTexture.loadFromRenderedText("DIT IS EEN STUK TEXT", textColor)){
            printf_s("Failed to render text texture\n");
            success = false;
        }
    }


    if (!gTexture.loadFromFile("hello2.png"))
    {
        printf_s("failed load sprites");
        success = false;
    } else {
        gTexture.setBlendMode(SDL_BLENDMODE_BLEND);

                //Set top left sprite
        gSpriteClips[ 0 ].x =   0;
        gSpriteClips[ 0 ].y =   0;
        gSpriteClips[ 0 ].w = 100;
        gSpriteClips[ 0 ].h = 100;

        gButtons[0] = LButton();

        //Set top right sprite
        gSpriteClips[ 1 ].x = 100;
        gSpriteClips[ 1 ].y =   0;
        gSpriteClips[ 1 ].w = 100;
        gSpriteClips[ 1 ].h = 100;

        gButtons[1] = LButton();
        
        //Set bottom left sprite
        gSpriteClips[ 2 ].x =   0;
        gSpriteClips[ 2 ].y = 100;
        gSpriteClips[ 2 ].w = 100;
        gSpriteClips[ 2 ].h = 100;

        gButtons[2] = LButton();

        //Set bottom right sprite
        gSpriteClips[ 3 ].x = 100;
        gSpriteClips[ 3 ].y = 100;
        gSpriteClips[ 3 ].w = 100;
        gSpriteClips[ 3 ].h = 100;

        gButtons[3] = LButton();
    }

    return success;
}

void close(){
    gBackgroundTexture.free();
    gTexture.free();
    gTextTexture.free();
    TTF_CloseFont(gFont);
    gFont = NULL;
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(screen);
    sdlRenderer = NULL;
    screen = NULL;
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
}

SDL_Texture* loadTexture(std::string path){
    SDL_Texture* newTexture = NULL;

    newTexture = IMG_LoadTexture(sdlRenderer, path.c_str());

    return newTexture;
}

int main(int argc, char* args[])
{
    if (init())
    {
        printf_s("Initialized SDL.\n");
    } else {
        printf_s("Could not initialize SDL. exiting...\n");
        return 0;
    }

    if (loadMedia())
    {
        printf_s("Loaded media.\n");
    } else {
        printf_s("Failed loading media files.\n");
    }

    LButtonSprite sprite = BUTTON_SPRITE_MOUSE_DOWN;

    while (1){

        SDL_Event e;
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                break;
            } else if (e.type == SDL_KEYDOWN)
            {
                
            }
        }

        const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
        if (currentKeyStates[SDL_SCANCODE_UP])
        {
            sprite = BUTTON_SPRITE_MOUSE_OUT;
        } else if (currentKeyStates[SDL_SCANCODE_DOWN])
        {
            sprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
        } else if (currentKeyStates[SDL_SCANCODE_LEFT])
        {
            sprite = BUTTON_SPRITE_MOUSE_UP;
        } else {
            sprite = BUTTON_SPRITE_MOUSE_DOWN;
        }

        SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(sdlRenderer);
        
        //SDL_Rect* currentClip = &gSpriteClips[frame / 4];
        //gTexture.render((SCREEN_WIDTH - currentClip->w) / 2, (SCREEN_HEIGHT - currentClip->h)/2, currentClip);
        gTexture.render(0,0,&gSpriteClips[sprite]);

        SDL_RenderPresent(sdlRenderer);
    }


    return 0;
}