#ifndef ENTITY
#define ENTITY

#include "utils.h"
#include <string>
//players and mobs
using namespace std;
class entity{
	public:
		string name;
		uint32_t dlvl, x, y;
		entity(string name = "Player");
		vector<uint8_t> save() const;
		void unpack(vector<uint8_t> block);
};

#endif

