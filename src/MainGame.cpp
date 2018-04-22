//#pragma once
#include "MainGame.h"
#include "Debug.h"
#include "Sprite.h"

MainGame::MainGame() : _screenWidth(1024), _screenHeight(768), _time(0), _window(nullptr), _gameState(GameState::PLAY)
{
    
}

MainGame::~MainGame()
{

}


void MainGame::run()
{
    initSystem();

    _sprite.init( -1.0f, -1.0f, 2.0f, 2.0f);

    MainGame::gameLoop();
}
void MainGame::initSystem()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    _window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
    if (_window == nullptr)
       Error("SDL Window could not be created"); 

    SDL_GLContext glContext = SDL_GL_CreateContext(_window);
    if (glContext == nullptr)
        Error("SDL_GL context could not be created!");

    GLenum error = glewInit();
    if(error != GLEW_OK)
        Error("Could not init GLEW");

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    glClearColor(1.0f,0.0f,0.0f,1.0f);
    _gameState = GameState::PLAY;
    
    initShaders();
}

void MainGame::initShaders()
{
    _colorProgram.compileShaders("src/Shaders/cShader.vert","src/Shaders/cShader.frag");
    _colorProgram.addAttribute("vertexPosition");
    _colorProgram.addAttribute("vertexColor");
    _colorProgram.linkShaders();
}

void MainGame::gameLoop()
{
    while (_gameState == GameState::PLAY){
        processInput();
        _time += 0.01;
        drawGame();
    }
}
void MainGame::processInput()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type){
            case SDL_QUIT:
                _gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                std::cout << event.motion.x << " " << event.motion.y << std::endl;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    _gameState = GameState::EXIT;
                    break;
                }
                break;
        }
    }

}

void MainGame::drawGame()
{
    
    
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _colorProgram.use();
    
    GLuint timeLocation = _colorProgram.getUniformLocation("time");
    glUniform1f( timeLocation, _time);
    
    _sprite.draw();
    
    _colorProgram.unUse();

    SDL_GL_SwapWindow(_window);
}
