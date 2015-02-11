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

#define USING_GAME_CODE
//#define other
using namespace std;

#ifdef other
void printArray(int * arr, int count) {
    cout << "{";
    for (int i = 0; i < count; ++i) {
        if (i > 0)
            cout << ", ";
        cout << arr[i];
    }
    cout << "}" << endl;
}
#endif

int main() {

#ifdef other
    
    int a;
    
    cout << "Enter amount of numbers to add up" << endl;
    cin >> a;
    
    int data[a];
    cout << "Enter numbers:" << endl;
    for (int i = 0; i < a; ++i) {
        cin >> data[i];
    }
    printArray(data, a);
    
    int total;
    for (int j = 0; j < a; j++) {
        total += data[j];
    }
    cout << total << endl;
    
#endif
    
    
    
#ifdef USING_GAME_CODE
    char map[ 50 * 15 + 1 ] =
        "##################################################"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#           #                                    #"
        "#                       #                        #"
        "#                       #                        #"
        "#                       #                        #"
        "#                                                #"
        "#                                                #"
        "#                                                #"
        "#            #                                   #"
        "#                                                #"
        "##################################################";
    
    Game g(map, Vector2(50, 15));
    long long lastKnownTime = platform_upTimeMS(), now, msPassed;
    while (g.running) {
        g.draw();
        g.throttleCode(20);
        g.getUserInput();
        now = platform_upTimeMS();
        msPassed = now - lastKnownTime;
        g.update((int)msPassed);
        lastKnownTime = now;
        printf("%d       ", (int)msPassed);
    }
#endif
    return 0;
}
