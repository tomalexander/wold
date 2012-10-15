/*
 * Copyright (c) 2012 Tom Alexander, Tate Larsen
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#pragma once
#include "topaz.h"
#include "unit.h"
#include <unordered_map>

enum agent_actions {ATTACK, MINE, FARM, HARVEST_WOOD, HARVEST_STONE, REST, TRIBUTE, AGENT_ACTIONS_SIZE}; /**< The final element in the actions enum is merely to get the quantity of elements */

class agent : public topaz::unit
{
  public:
    agent();
    ~agent();
    
  private:
    u64 master;                 /**< Game Object ID of agent this agent is a vassal of */

    //Quantities of Resources
    float iron;
    float wood;
    float food;
    float stone;
    float water;

    std::unordered_map<u64, float> opinions_of_others; /**< Opinions held towards other agents this agent has met */

    float energy;
    float proficiency[AGENT_ACTIONS_SIZE];
    topaz::point home_location;
    float happiness;
};
