//
//  main.cpp
//  Console Game
//
//  Created by Lax26 on 1/7/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "platform_conio.h"
#include "vector2.h"
#include "entity.h"
#include "game.h"

using namespace std;

int main() {
    char map[ 50 * 15 + 1 ] =
        "##################################################"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                 "
        "##################################################";
    
    Game g(map, Vector2(50, 15));
    long long lastKnownTime = platform_upTimeMS(), now, msPassed;
    while (g.running) {
        platform_setColor(12, 0);
        g.draw();
        g.throttleCode(20);
        g.getUserInput();
        now = platform_upTimeMS();
        msPassed = now - lastKnownTime;
        g.update((int)msPassed);
        lastKnownTime = now;
    }

    return 0;
}
