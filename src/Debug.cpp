#include "Debug.h"

#include <iostream>
#include <SDL2/SDL.h>

void Error(std::string errorString)
{
	std::cout << errorString << std::endl;
	std::cout << "Enter any key to quit...";
	int tmp;
	std::cin >> tmp;
	SDL_Quit();
}

void Log(std::string logString)
{
	std::cout << logString << std::endl;
}
