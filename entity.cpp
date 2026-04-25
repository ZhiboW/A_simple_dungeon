#include "entity.h"

entity::entity(string name){
	this->name = name;
	dlvl = 1;
	x = 0;
	y = 0;
}

vector<uint8_t> entity::save() const{
	vector<uint8_t> block;
	
	uint32_t namelen = (uint32_t)name.size();
    push_to_block(block, namelen);
    block.insert(block.end(), name.begin(), name.end());

	push_to_block(block, dlvl);
	push_to_block(block, x);
	push_to_block(block, y);
	
	return block;
}

void entity::unpack(vector<uint8_t> block){
	if (block.empty()) return;
	
	size_t cursor = 0;
	
	uint32_t namelen;
    read_from_block(block, cursor, namelen);
    
    name.assign(reinterpret_cast<const char*>(&block[cursor]), namelen);
    cursor += namelen;
    
    read_from_block(block, cursor, dlvl);
    read_from_block(block, cursor, x);
    read_from_block(block, cursor, y);
}
