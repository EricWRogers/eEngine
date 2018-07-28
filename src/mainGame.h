//#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "Sprite.h"
#include "GLSLProgram.h"
#include "GLTexture.h"
#include "Window.h"
#include "Debug.h"
#include "lua.hpp"
  
enum class GameState {PLAY, EXIT};

class mainGame
{
public:
    void run();
    

private:
    void initSystem();
    void initShaders();
    void gameLoop();
    void processInput();
    void drawGame();
    void calculateFPS();
    
    Window _window;
    int _screenWidth = 1024;
    int _screenHeight = 768;
    GameState _gameState;

    std::vector <Sprite*> _sprite;

    GLSLProgram _colorProgram;

    float _fps;
    float _maxFPS = 120.0f;;
    float _frameTime;
    float _time = 0;
    

};
