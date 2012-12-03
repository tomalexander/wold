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
#define MAP_SIZE 1024
#define TERRAIN_SCALE 1024.0f
#define TERRAIN_HEIGHT 25.0f
#define VISIBLE_RANGE 30.0f

#include "map_grid.h"
#include "modules/include_all_modules.h"
#include "world.h"
#include "topaz.h"

#include <map>
#include <string>
#include <vector>

world::world()
{
    // Set up Random seed generation
    srand ( time(NULL) );
    world_seed = rand();

    init();
}

world::world(const int& seed) : world_seed(seed)
{
    init();
}

world::~world()
{
    delete terrain;

    // Delete Resource Maps
    std::map<std::string,noise::map_grid*>::iterator del_it = resources.begin();
    for(; del_it != resources.end(); ++del_it)
    {
        delete del_it->second;
        del_it->second = 0;
    }
}

void world::init()
{
    // Hold various map_grids as we push them into the map of resources
    noise::map_grid* mg_ptr = new noise::map_grid(MAP_SIZE,MAP_SIZE);

    // Generate Terrain
    noise::perlin_mod* terrain_gen = new noise::perlin_mod(0.5f, 6, world_seed);
    mg_ptr->set_module(terrain_gen);
    mg_ptr->generate(0,0);
    float* data = mg_ptr->get_data();

    // Keep track of the terrain (for height sampling)
    resources.insert(std::pair<std::string, noise::map_grid*>("terrain", mg_ptr));

    terrain = new topaz::terrain(MAP_SIZE, MAP_SIZE, data, TERRAIN_HEIGHT/TERRAIN_SCALE);
    terrain->set_scale(TERRAIN_SCALE);
    terrain->finalize();

    // Generate Resource Maps

    // Iron Distribution
    noise::perlin_mod* iron_gen = new noise::perlin_mod(0.5f, 6, world_seed + 1);
    mg_ptr->set_module(iron_gen);
    mg_ptr->generate(0,0);
    resources.insert(std::pair<std::string, noise::map_grid*>("iron", mg_ptr));
    
    // Wood Distribution
    mg_ptr = new noise::map_grid(MAP_SIZE,MAP_SIZE);

    noise::perlin_mod* wood_gen = new noise::perlin_mod(0.5f, 6, world_seed + 2);
    mg_ptr->set_module(wood_gen);
    mg_ptr->generate(0,0);
    resources.insert(std::pair<std::string, noise::map_grid*>("wood", mg_ptr));

    // Food Distribution
    mg_ptr = new noise::map_grid(MAP_SIZE,MAP_SIZE);

    noise::perlin_mod* food_gen = new noise::perlin_mod(0.5f, 6, world_seed + 3);
    mg_ptr->set_module(food_gen);
    mg_ptr->generate(0,0);
    resources.insert(std::pair<std::string, noise::map_grid*>("food", mg_ptr));

    // Stone Distribution
    mg_ptr = new noise::map_grid(MAP_SIZE,MAP_SIZE);

    noise::perlin_mod* stone_gen = new noise::perlin_mod(0.5f, 6, world_seed + 4);
    mg_ptr->set_module(stone_gen);
    mg_ptr->generate(0,0);
    resources.insert(std::pair<std::string, noise::map_grid*>("stone", mg_ptr));

    // Water Distribution
    mg_ptr = new noise::map_grid(MAP_SIZE,MAP_SIZE);

    noise::perlin_mod* water_gen = new noise::perlin_mod(0.5f, 6, world_seed + 5);
    mg_ptr->set_module(water_gen);
    mg_ptr->generate(0,0);
    resources.insert(std::pair<std::string, noise::map_grid*>("water", mg_ptr));
}

node_info world::sample_loc(const float& x_loc, const float& y_loc)
{
    // Go through and grab the resource values for a given x,y coordinate
    node_info res_samp;
    res_samp.x = x_loc;
    res_samp.y = y_loc;
    res_samp.z = resources.find("terrain")->second->get_val(x_loc, y_loc);
    res_samp.health = 0.0f;
    res_samp.iron = resources.find("iron")->second->get_val(x_loc, y_loc);
    res_samp.wood = resources.find("wood")->second->get_val(x_loc, y_loc);
    res_samp.food = resources.find("food")->second->get_val(x_loc, y_loc);
    res_samp.stone = resources.find("stone")->second->get_val(x_loc, y_loc);
    res_samp.water = resources.find("water")->second->get_val(x_loc, y_loc);
    return res_samp;
}

std::vector<node_info> world::visible_things(agent* cur_agent)
{
    std::vector<node_info> things;
    topaz::point center = cur_agent->get_location();
    // Sample all locations within VISIBLE_RANGE units
    for(int x = center.x() - VISIBLE_RANGE; x < center.x() + VISIBLE_RANGE; ++x)
    {
        if(x < 0 || x >= MAP_SIZE) continue;
        for(int y = center.y() - VISIBLE_RANGE; y < center.y() + VISIBLE_RANGE; ++y)
        {
            if(y < 0 || y >= MAP_SIZE) continue;
            topaz::point tmp((float)x, (float)y, center.z());
            if(cur_agent->get_distance_to(tmp) < VISIBLE_RANGE) things.push_back(sample_loc(tmp.x(), tmp.y()));
        }
    }

    // Add all agents within VISIBLE_RANGE units
    // TODO

    return things;
}
