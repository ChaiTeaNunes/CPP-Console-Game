//
//  game.h
//  Console Game
//
//  Created by Lax26 on 1/14/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "platform_conio.h"
#include "vector2.h"
#include "entity.h"
#include "aabb.h"
#include "enemy.h"

class Game {
public:
    char * map;

public:
    Entity player;
    static const int ENEMY_COUNT = 5;
    Enemy enemies[ENEMY_COUNT];
    Vector2 size;
    AABB playArea;
    char input;
    bool running;
    Game(char * map, Vector2 size) : player(Vector2(4,5), '@'), size(size), running(true), playArea(Vector2(0,0), size), map(map)
    {
        for (int i = 0; i < ENEMY_COUNT; ++i) {
            enemies[i] = Enemy(Vector2(i * 3 + 1, i * 2 + 1), '!', &player, 200 + (i * 200));
        }
    }
    
    int mapIndex(int row, int col) { return row * size.x + col; }
    
    void draw () {
        platform_move(0,0);
        for (int row = 0; row < size.y; ++row) {
            for (int col = 0; col < size.x; ++col) {
                bool enemyHere = false;
                for (int i = 0; i < ENEMY_COUNT; i++) {
                    enemyHere = enemies[i].position.equals(col, row);
                    if (enemyHere) {
                        putchar(enemies[i].icon);
                        break;
                    }
                }
                if (!enemyHere) {
                    if (player.position.equals(col, row)) {
                        putchar(player.icon);
                    } else {
                        putchar(map[mapIndex(row, col) ]);
                    }
                }
            }
            putchar('\n');
        }
        putchar('\n');
    }
    
    bool emptyLocation(int row, int col, int ignore) {
        if (map[mapIndex(row, col)] == ' ') {
            bool filled = false;
            for (int i = 0; i < ENEMY_COUNT; i++) {
                if (i != ignore
                    && enemies[i].position.x == col
                    && enemies[i].position.y == row) {
                    filled = true;
                    break;
                }
            }
            return !filled;
        }
        return false;
    }
    
    void update(int msPassed) {
        switch (input) {
            case 27:
                running = false;
                break;
                
            default:
                player.nextMove = input;
                break;
        }
        Vector2 oldpp = player.position;
        player.Update(msPassed);
        if (!playArea.Contains(player.position)
            || map[mapIndex(player.position.y, player.position.x)] != ' ') {
            player.position = oldpp;
        }
        
        for (int i = 0; i < ENEMY_COUNT; i++){
            Vector2 oldep = enemies[i].position;
            enemies[i].Update(msPassed);
            if (!playArea.Contains(enemies[i].position)
                || !emptyLocation(enemies[i].position.y, enemies[i].position.x, i)) {
                //|| map[mapIndex(enemies[i].position.y, enemies[i].position.x)] != ' ') {
                enemies[i].position = oldep;
            }
        }
        
    }
    
    void getUserInput() {
        if (platform_kbhit()) {
            input = platform_getchar();
            
        } else {
            input = -1;
            
//            if (e1.position.x == player.position.x && e1.position.y == player.position.y) {
//                running = false;
//            }
//            if (e2.position.x == player.position.x && e2.position.y == player.position.y) {
//                running = false;
//            }
        }
    }
    
    void throttleCode(int msDelay) {
        long long now = platform_upTimeMS();
        while (!platform_kbhit() && platform_upTimeMS() < now + msDelay) {
            platform_sleep(1);
        }
    }
    
};
