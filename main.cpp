//
//  main.cpp
//  Console Game
//
//  Created by Wattie on 1/7/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "platform_conio.h"

int main() {
    int x = 4, y = 5, width = 20, height = 15;
    char input;
    bool running = true;
    char icon = 'a', border = '#', platform = '.';
    //game loop
    while (running) {
        // render, draw
        platform_move(5,0);
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                if (row == y && col == x) {
                    putchar(icon);
                } else {
                    if (col == width - 1 || row == height - 1){
                        putchar(border);
                    }
                    else if (col == 0 || row == 0){
                        putchar(border);
                    }
                    else {
                        putchar(platform);
                    }
                }
            }
            putchar('\n');
        }
        putchar('\n');
        // throttle code
        long long now = platform_upTimeMS();
        while (!platform_kbhit() && platform_upTimeMS() < now + 20) {
            platform_sleep(1);
        }
        // input - get input from the user
        if (platform_kbhit()) {
            input = platform_getchar();
        } else {
            input = -1;
        }
        if (input == -1) {
            icon++;
            if (icon > 'z')
                icon = 'a';
        }
        // update - process input from the user and update the game state
        switch (input) {
            case 'w': --y; break;
            case 'a': --x; break;
            case 's': ++y; break;
            case 'd': ++x; break;
            case 'q': running = false; break;
        }
        if (y == height - 1) {
            y = y - 1;
        } else if (y == 0) {
            y = y + 1;
        } else if (x == width - 1) {
            x = x - 1;
        } else if (x == 0) {
            x = x + 1;
        }
    }
    return 0;
}
