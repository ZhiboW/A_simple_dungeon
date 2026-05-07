#ifndef INVENTORY
#define INVENTORY

#include <vector>
#include <unordered_map>
#inclue <string>
#include "utils.h"
using namespace std;

class item{
	public:
		string name; // item name
		unsigned int item_id, type; // item identifier, item type, search by type, number from item list
		int cost, resell;
		unordered_map statblock<int, double>; //variable sized stat block, flexible stat allocation
		void read_item(unsigned int type, unsigned int id);
};

class inv{
	public:
		string bagsize;
		unsigned long item_slots;
		unsigned long money;
		vector<int> item_count;
		vector<item> item_list;
		vector<int> equipped;
		int size_allocator();
		void equipper(item, item &);
		void unpack(vector<uint8_t> block);
};

#endif
