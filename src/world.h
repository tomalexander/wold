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
#include "terrain.h"
#include "agent.h"

#include <map>
#include <string>
#include <vector>

namespace noise
{
    class map_grid;
}

class agent;

typedef struct {
    float x, y, z;
    float health;
    float iron, wood, food, stone, water;
} node_info;

class world
{
  public:
    world();
    world(const int& seed);
    ~world();
    
    void init();

    std::vector<node_info> visible_things(agent* cur_agent);
    node_info sample_loc(const float& x_loc, const float& y_loc);

  private:
    int world_seed;
    topaz::terrain* terrain;
    std::map<std::string, noise::map_grid*> resources;
};
