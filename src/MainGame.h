#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>

#include "Sprite.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
    MainGame();
    ~MainGame();

    void run();
    

private:
    void initSystem();
    void gameLoop();
    void processInput();
    void drawGame();

    
    SDL_Window* _window;
    int _screenWidth, _screenHeight;
    GameState _gameState;

    Sprite _sprite;
};