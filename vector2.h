//
//  vector2.h
//  Console Game
//
//  Created by Lax26 on 1/14/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#ifndef Console_Game_vector2_h
#define Console_Game_vector2_h

class Vector2
{
public:
    int x, y;
    Vector2() : x(0), y(0) { }
    Vector2(int x, int y) : x(x), y(y) { }
    bool equals(int x, int y) {
        return this -> x == x && this -> y == y;
    }
};

#endif
