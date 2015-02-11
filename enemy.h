//
//  h
//  Console Game
//
//  Created by Lax26 on 1/22/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#ifndef Console_Game_enemy_h
#define Console_Game_enemy_h

#pragma once

class Enemy : public Entity {
public:
    int timer, moveDelay;
    /** pointer to the entity that is being followed */
    Entity * followed;
    
    /** automatically calls the Entity (parent class) default constructor */
    Enemy() : followed(0), timer(0), moveDelay(500) {}
    
    /** explicitly calls the Entity (parent class) constructor */
    Enemy(Vector2 position, char icon, Entity * player, int delay)
        : Entity(position, icon), followed(player), timer(0), moveDelay(delay) {}
    
    void Update(int msPassed) {
        
        timer += msPassed;
        if(timer >= moveDelay) {
            timer -= moveDelay;
            if (followed) {
                if (followed -> position.y < position.y) nextMove = 'w';
                if (followed -> position.x < position.x) nextMove = 'a';
                if (followed -> position.y > position.y) nextMove = 's';
                if (followed -> position.x > position.x) nextMove = 'd';
            }
            Entity::Update(msPassed);
        }
        else {
            nextMove = 0;
        }
    }
};


#endif
