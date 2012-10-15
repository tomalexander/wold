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
#include "agent.h"
#include "topaz.h"
#include "egg_parser.h"
#include <unordered_map>
#include <list>

namespace
{
    std::unordered_map<u64, agent*> agents;
    agent* get_agent_from_id(u64 id)
    {
        auto it = agents.find(id);
        return (it == agents.end() ? nullptr : it->second);
    }

    std::list<agent*> find_top_level_agents()
    {
        std::list<agent*> ret;
        for (const std::pair<u64, agent*> & entry : agents)
        {
            if (entry.second->id == entry.second->get_master())
            {
                ret.push_back(entry.second);
            }
        }
        return ret;
    }
}

agent::agent() :
    master(id)
{
    set_model(topaz::get_model("panda-model"));
    set_scale(0.005);
    agents.insert(make_pair(id, this));
    // topaz::add_cleanup_function([=]() {delete this;});
}

agent::~agent()
{
    agents.erase(id);
}
