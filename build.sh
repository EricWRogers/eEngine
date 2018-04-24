#!/bin/bash

g++ src/*.cpp -std=gnu++11 -lSDL2 -lGL -lGLEW -pthread -o bin/preAlPHA && ./bin/preAlPHA
