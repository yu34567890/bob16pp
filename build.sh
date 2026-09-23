#!/bin/sh
cc main.c decompiler.c gpu.c $(pkg-config --cflags --libs sdl3)  -ggdb -DDEBUG -Ofast -o gputest
