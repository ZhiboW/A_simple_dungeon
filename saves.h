#ifndef SAVES
#define SAVES

#include <fstream>
#include <map>
#include "map.h"
#include "entity.h"

namespace saves{
	void save_floor(const string& filename, dungeon& savemap);
	bool load_floor(const string& filename, uint32_t id, dungeon& loadmap);
	void save_player(const string& filename, const entity& player);
	bool load_player(const string& filename, entity& player);
}

#endif
