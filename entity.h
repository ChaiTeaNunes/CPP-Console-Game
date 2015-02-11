//
//  entity.h
//  Console Game
//
//  Created by Lax26 on 1/14/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#ifndef Console_Game_entity_h
#define Console_Game_entity_h

class Entity {
public:
    Vector2 position;
    char icon;
    char nextMove;
    Entity() : icon('?'), nextMove(0) { }
    Entity(Vector2 position, char icon) : position(position), icon(icon) { }
    
    void Update(int msPassed) {
        switch (nextMove) {
            case 'w':
                position.y--;
                break;
            case 'a':
                position.x--;
                break;
            case 's':
                position.y++;
                break;
            case 'd':
                position.x++;
                break;
        }
    }
};

#endif
