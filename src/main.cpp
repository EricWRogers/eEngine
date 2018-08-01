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
#include "Camera2D.h"
#include "SpriteBatch.h"
#include "ResourceManager.h"

#include "lua5.3/lua.hpp"

void initShaders();
void processInput();
void drawGame();
void calculateFPS();

enum class GameState {PLAY, EXIT};

int screenWidth = 1024;
int screenHeight = 768;

const int NUM_SDL_SCANCODES = 512;
bool keys[NUM_SDL_SCANCODES];
bool prevKeys[NUM_SDL_SCANCODES];

Window window;

Camera2D camera;

GameState gameState;

std::vector <Sprite*> sprite;

GLSLProgram colorProgram;

SpriteBatch _spriteBatch;

const char * const CONFIG_FILE = "config.lua";

lua_State* luaState;

float fps;
float maxFPS = 120.0f;;
float frameTime;
float gameTime = 0.0f;

char stringBuilderBuffer[512];

void initShaders() {
    colorProgram.compileShaders("src/Shaders/cShader.vert","src/Shaders/cShader.frag");
    colorProgram.addAttribute("vertexPosition");
    colorProgram.addAttribute("vertexColor");
    colorProgram.addAttribute("vertexUV");
    colorProgram.linkShaders();
}

void processInput()
{
    const float CAMERA_SPEED = 10.0f;
    const float SCALE_SPEED = 0.1f;
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type){
            case SDL_QUIT:
                gameState = GameState::EXIT;
                break;
            case SDL_MOUSEMOTION:
                // This event is only called when mouse is moved
                //std::cout << event.motion.x << " " << event.motion.y << std::endl;
                break;
            case SDL_KEYUP:
                keys[event.key.keysym.scancode] = false;
                //prevKeys[event.key.keysym.scancode] = true;
            break;
            case SDL_KEYDOWN:
                keys[event.key.keysym.scancode] = true;
                //prevKeys[event.key.keysym.scancode] = false;
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        gameState = GameState::EXIT;
                        break;
                    case SDLK_w:
                        camera.setPosition(camera.getPosition() + glm::vec2(0.0f, 10.0f));
                        break;
                    case SDLK_s:
                        camera.setPosition(camera.getPosition() + glm::vec2(0.0f, -10.0f));
                        break;
                    case SDLK_a:
                        camera.setPosition(camera.getPosition() + glm::vec2(-10.0f, 0.0f));
                        break;
                    case SDLK_d:
                        camera.setPosition(camera.getPosition() + glm::vec2(10.0f, 0.0f));
                        break;
                    case SDLK_q:
                        camera.setScale(camera.getScale() + SCALE_SPEED);
                        break;
                    case SDLK_e:
                        camera.setScale(camera.getScale() - SCALE_SPEED);
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
    // Set the camera matrix
    GLuint pLocation = colorProgram.getUniformLocation("P");
    glm::mat4 cameraMatrix = camera.getCameraMatrix(); 
    glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

    _spriteBatch.begin();

    glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);
    glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
    static GLTexture texture = ResourceManager::getTexture("src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
    Color color;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255;
    
    _spriteBatch.draw(pos, uv, texture.id, 0.0f, color);
    _spriteBatch.draw(pos + glm::vec4(50.0f, 0.0f, 0.0f, 0.0f), uv, texture.id, 0.0f, color);
   
    _spriteBatch.end();
  
    _spriteBatch.renderBatch();
    
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

////////////////////
//// lua_State* ////
////////////////////

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

///////////////////////
//// LUA FUNCTIONS ////
///////////////////////

// Delete This 
int twoSprite(lua_State* luaState) {
    sprite.push_back(new Sprite());
    sprite.back()->init( 0.0f, 0.0f, screenWidth / 2, screenHeight / 2, "src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
    
    sprite.push_back(new Sprite());
    sprite.back()->init(  screenWidth / 2, 0.0f, screenWidth / 2, screenHeight / 2, "src/Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
    return 2;
}

int CreateWindow(lua_State* state) {

    screenWidth = (int)lua_tointeger(state, 1);
    screenHeight = (int)lua_tointeger(state, 2);
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
    // init camera
    camera.init(screenWidth, screenHeight);

    gameState = GameState::PLAY;

    initShaders();

    _spriteBatch.init();
    
    return 0;
}

int IsKeyDown(lua_State* state) {
    int scancode = (int)lua_tointeger(state, 1);
	lua_pushboolean(state, keys[scancode]);
    return 1;
}

int IsKeyReleased(lua_State* state) {
	int scancode = (int)lua_tointeger(state, 1);
	lua_pushboolean(state, !keys[scancode] && prevKeys[scancode]);
	return 1;
}

int IsKeyPressed(lua_State* state) {
	int scancode = (int)lua_tointeger(state, 1);
	lua_pushboolean(state, keys[scancode] && !prevKeys[scancode]);
	return 1;
}

int LuaLog(lua_State* state) {
    const char* text = lua_tostring(state, 1);
    Log(text);
    return 0;
}

void Start();
void Update();
void Draw();
// Ask James
void endUpdate() { SDL_memcpy(&prevKeys, &keys, NUM_SDL_SCANCODES); }

int main(int argc, char** argv) {

    //initSystem();
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
    
    lua_register(luaState, "IsKeyDown", IsKeyDown);
    lua_register(luaState, "IsKeyReleased", IsKeyReleased);
    lua_register(luaState, "IsKeyPressed", IsKeyPressed);
    lua_register(luaState, "twoSprite", twoSprite);
    lua_register(luaState, "CreateWindow", CreateWindow);
    lua_register(luaState, "Log", LuaLog);

    runScript(luaState, CONFIG_FILE);
    
    call(luaState, "Start");

    while (gameState == GameState::PLAY){
        float startTicks = SDL_GetTicks();

        processInput();
        gameTime += 0.01f;
        camera.update();
        call(luaState, "Update");
        drawGame();
        calculateFPS();
        endUpdate();
{
    SDL_memcpy(&prevKeys, &keys, NUM_SDL_SCANCODES);
}

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
