#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "Sprite.h"
#include "GLSLProgram.h"
#include "GLTexture.h"
#include "Window.h"
#include "Debug.h"
#include "lua.hpp"

void initSystem();
void initShaders();
//void gameLoop();
void processInput();
void drawGame();
void calculateFPS();

enum class GameState {PLAY, EXIT};

Window window;
int screenWidth = 1024;
int screenHeight = 768;
GameState gameState;

std::vector <Sprite*> sprite;

GLSLProgram colorProgram;

const char * const CONFIG_FILE = "config.lua";

lua_State* luaState;

float fps;
float maxFPS = 120.0f;;
float frameTime;
float gameTime = 0.0f;

char stringBuilderBuffer[512];

void initSystem()
{
    SDL_Init( SDL_INIT_EVERYTHING );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window.create("Game Engine", screenWidth, screenHeight, 0);

    gameState = GameState::PLAY;

    initShaders();
}

void initShaders() {
    colorProgram.compileShaders("src/Shaders/cShader.vert","src/Shaders/cShader.frag");
    colorProgram.addAttribute("vertexPosition");
    colorProgram.addAttribute("vertexColor");
    colorProgram.addAttribute("vertexUV");
    colorProgram.linkShaders();
}

void processInput()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type){
            case SDL_QUIT:
                gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                std::cout << event.motion.x << " " << event.motion.y << std::endl;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    gameState = GameState::EXIT;
                    break;
                }
                break;
        }
    }
}

void drawGame()
{
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    colorProgram.use();
    glActiveTexture(GL_TEXTURE0);
    
    GLint textureLocation = colorProgram.getUniformLocation("mySampler");
    glUniform1i(textureLocation, 0);
    
    GLuint timeLocation = colorProgram.getUniformLocation("time");
    glUniform1f( timeLocation, gameTime);
    
    for (int i = 0; i < sprite.size(); i++)
    {
        sprite[i]->draw();
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    colorProgram.unUse();

    window.swapBuffer();
}

void calculateFPS()
{
    static const int NUM_SAMPLES = 10;
    static float frameTimes[NUM_SAMPLES];
    static int currentFrame = 0;

    static float prevTicks = SDL_GetTicks();

    float currentTicks;
    currentTicks = SDL_GetTicks();

    frameTime = currentTicks - prevTicks;
    frameTimes[currentFrame % NUM_SAMPLES] = frameTime;

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
        fps = 1000.0f / frameTimeAverage;
    } else {
        fps = 60.0f;
    }
}

void printError(lua_State* state)
{
    Error(lua_tostring(state, -1));
    lua_pop(state, 1); // remove error message from top of stack
}

void runScript(lua_State* state, const char* file)
{
    int result = luaL_loadfile(state, file);

    if (result == LUA_OK)
    {
        // use pcall to execute the script.
        result = lua_pcall(state, 0, LUA_MULTRET, 0);

        if (result != LUA_OK)
        {
            printError(state);
        }
    }
    else
    {
        printError(state);
    }
}

void call(lua_State* state, const char* functionName)
{
    // pushes onto the stack the value of the global name of the lua function to be called.
    int type = lua_getglobal(state, functionName);

    if (type == LUA_TNIL)
    {
        snprintf(stringBuilderBuffer, sizeof(stringBuilderBuffer), "Attempted to call undefined Lua function: %s", functionName);
        Error(stringBuilderBuffer);
    }
    else if (lua_pcall(state, 0, 0, 0) != 0)
    {
        printError(state);
    }
}

int twoSprite(lua_State* luaState) {
    sprite.push_back(new Sprite());
    sprite.back()->init( -1.0f, -1.0f, 1.0f, 1.0f, "src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
    
    sprite.push_back(new Sprite());
    sprite.back()->init( 0.0f, -1.0f, 1.0f, 1.0f, "src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
    return 2;
}

int CreateWindow(lua_State* state) {

    int screenWidth = (int)lua_tointeger(state, 1);
    int screenHeight = (int)lua_tointeger(state, 2);
    const char* title = lua_tostring(state, 3);
	bool fullscreen = false;
	if (lua_gettop(state) >= 4)
	{
		fullscreen = lua_toboolean(state, 4) == 1;
	}

    SDL_Init( SDL_INIT_EVERYTHING );
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Add fullScreen
    window.create( title, screenWidth, screenHeight, 0);

    gameState = GameState::PLAY;

    initShaders();
    return 0;
}

int main(int argc, char** argv) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);

    lua_register(luaState, "twoSprite", twoSprite);
    lua_register(luaState, "CreateWindow", CreateWindow);

    runScript(luaState, CONFIG_FILE);
    
    call(luaState, "Start");


    while (gameState == GameState::PLAY){
        float startTicks = SDL_GetTicks();

        processInput();
        gameTime += 0.01f;
        drawGame();
        calculateFPS();

        static int frameCounter = 0;
        frameCounter++;
        if (frameCounter == 10) { 
            std::cout << fps << std::endl;
            frameCounter = 0;
        }

        float frameTicks = SDL_GetTicks() - startTicks;
        if (1000.0f / maxFPS > frameTicks) {
            SDL_Delay(1000.0f / maxFPS - frameTicks);
        }
    }

    return 0;
}
