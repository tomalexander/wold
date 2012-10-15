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
#define TERRAIN_SCALE 32.0f

#include "map_grid.h"
#include "modules/include_all_modules.h"
#include "world.h"
#include "topaz.h"

namespace
{
    noise::map_grid mg(MAP_SIZE,MAP_SIZE);
}

world::world()
{
    //Set up Random seed generation
    srand ( time(NULL) );
    world_seed = rand();

    //Generate Terrain
    noise::perlin_mod* p_m = new noise::perlin_mod(0.5f, 6, world_seed);
    mg.set_module(p_m);
    mg.generate(0,0);
    float* data = mg.get_data();

    terrain = new topaz::terrain(MAP_SIZE, MAP_SIZE, data, 1.0f/TERRAIN_SCALE);
    terrain->set_scale(TERRAIN_SCALE);
    terrain->finalize();
}

world::~world()
{
    delete terrain;
}
