//#pragma once
#include "MainGame.h"
#include "Debug.h"
#include "Sprite.h"

MainGame::MainGame() : _screenWidth(1024), _screenHeight(768), _time(0), _gameState(GameState::PLAY), _maxFPS(120.0f)
{
    
}

MainGame::~MainGame()
{

}


void MainGame::run()
{
    initSystem();
    // initialize our sprite
    _sprite.push_back(new Sprite());
    _sprite.back()->init( -1.0f, -1.0f, 1.0f, 1.0f, "src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
    
    _sprite.push_back(new Sprite());
    _sprite.back()->init( 0.0f, -1.0f, 1.0f, 1.0f, "src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

    // _playerTexture = ImageLoader::loadPNG("src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

    MainGame::gameLoop();
}
void MainGame::initSystem()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    _window.create("Game Engine", _screenWidth, _screenHeight, 0);
    
    _gameState = GameState::PLAY;

    initShaders();
}

void MainGame::initShaders()
{
    _colorProgram.compileShaders("src/Shaders/cShader.vert","src/Shaders/cShader.frag");
    _colorProgram.addAttribute("vertexPosition");
    _colorProgram.addAttribute("vertexColor");
    _colorProgram.addAttribute("vertexUV");
    _colorProgram.linkShaders();
}

void MainGame::gameLoop()
{
    while (_gameState == GameState::PLAY){
        float startTicks = SDL_GetTicks();

        processInput();
        _time += 0.01;
        drawGame();
        calculateFPS();

        static int frameCounter = 0;
        frameCounter++;
        if (frameCounter == 10) { 
            std::cout << _fps << std::endl;
            frameCounter = 0;
        }

        float frameTicks = SDL_GetTicks() - startTicks;
        if (1000.0f / _maxFPS > frameTicks) {
            SDL_Delay(1000.0f / _maxFPS - frameTicks);
        }
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
    glActiveTexture(GL_TEXTURE0);
    
    GLint textureLocation = _colorProgram.getUniformLocation("mySampler");
    glUniform1i(textureLocation, 0);
    
    GLuint timeLocation = _colorProgram.getUniformLocation("time");
    glUniform1f( timeLocation, _time);
    
    for (int i = 0; i < _sprite.size(); i++)
    {
        _sprite[i]->draw();
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    _colorProgram.unUse();

    _window.swapBuffer();
}

void MainGame::calculateFPS()
{
    static const int NUM_SAMPLES = 10;
    static float frameTimes[NUM_SAMPLES];
    static int currentFrame = 0;

    static float prevTicks = SDL_GetTicks();

    float currentTicks;
    currentTicks = SDL_GetTicks();

    _frameTime = currentTicks - prevTicks;
    frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;
    
    prevTicks = currentTicks;

    int count;
    currentFrame++;
    if (currentFrame < NUM_SAMPLES)
    {
        count = currentFrame;
    } else 
    {
        count = NUM_SAMPLES;    
    }

    float frameTimeAverage = 0;
    for (int i = 0; i < count; i++) {
        frameTimeAverage += frameTimes[i];
    }

    frameTimeAverage /= count;

    if (frameTimeAverage > 0) {
        _fps = 1000.0f / frameTimeAverage;
    } else {
        _fps = 60.0f;
    }
}
