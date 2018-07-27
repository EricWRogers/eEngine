//#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <vector>

#include "Sprite.h"
#include "GLSLProgram.h"
#include "GLTexture.h"
#include "Window.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
    MainGame();
    ~MainGame();

    void run();
    

private:
    void initSystem();
    void initShaders();
    void gameLoop();
    void processInput();
    void drawGame();
    void calculateFPS();
    
    Window _window;
    int _screenWidth, _screenHeight;
    GameState _gameState;

    std::vector <Sprite*> _sprite;

    GLSLProgram _colorProgram;
    
    float _fps;
    float _maxFPS;
    float _frameTime;
    float _time;
};
