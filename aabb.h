//
//  aabb.h
//  Console Game
//
//  Created by Lax26 on 1/21/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#ifndef Console_Game_aabb_h
#define Console_Game_aabb_h

//Axis-Aligned Bounding Box

#include "vector2.h"

class AABB
{
public:
    Vector2 min, max;
    
    bool Contains(Vector2 p) {
        if (p.x >= min.x && p.x < max.x
            && p.y >= min.y && p.y < max.y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    AABB (Vector2 cornerA, Vector2 cornerB) {
        // TODO #define MIN, MAX
        min.x = (cornerA.x < cornerB.x) ? cornerA.x : cornerB.x;
        min.y = (cornerA.y < cornerB.y) ? cornerA.y : cornerB.y;
        max.x = (cornerA.x > cornerB.x) ? cornerA.x : cornerB.x;
        max.y = (cornerA.y > cornerB.y) ? cornerA.y : cornerB.y;
        //  TODO #undef MIN, MAX
    }
};

#endif
