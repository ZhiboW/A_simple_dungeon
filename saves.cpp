#include "saves.h"
namespace saves{

	void save_floor(const string& filename, dungeon& savemap) {
	    map<uint32_t, vector<uint8_t>> map_block;
	    //load
	    ifstream infile(filename, ios::binary);
	    if(infile){
	    	uint32_t mapsize, floorid;
            while(infile.read((char*)&mapsize, 4)){
                vector<uint8_t> block(mapsize);
                if(!infile.read((char*)block.data(), mapsize)) break;
                
                memcpy(&floorid, block.data(), 4);
                map_block[floorid] = block;
            }
            infile.close();
        }

        map_block[savemap.level] = savemap.save();

        ofstream outfile(filename, ios::binary | ios::trunc);
        for(auto const& i : map_block){
            const vector<uint8_t>& block = i.second;
            uint32_t mapsize = (uint32_t)block.size();
            outfile.write((char*)&mapsize, 4);
            outfile.write((char*)block.data(), mapsize);
        }
    }
    
    bool load_floor(const string& filename, uint32_t id, dungeon& loadmap){
        ifstream infile(filename, ios::binary);
        if (!infile) return false;
        
        uint32_t mapsize, i;
        while (infile.read((char*)&mapsize, 4)) {
            if(!infile.read((char*)&i, 4)) break;

            if (i == id) {
                vector<uint8_t> block(mapsize);
                memcpy(block.data(), &i, 4);
                infile.read((char*)block.data() + 4, mapsize - 4);
                loadmap.unpack(block);
                return true;
            } else {
                infile.seekg(mapsize - 4, ios::cur);
            }
        }
        return false;
    }
    
    void save_player(const string& filename, const entity& player){
    	vector<uint8_t> block = player.save();
    	ofstream outfile(filename, ios::binary | ios::trunc);
    	if (outfile) {
	        uint32_t size = (uint32_t)block.size();
	        outfile.write((char*)& size, 4);
	        outfile.write((char*)block.data(), size);
	    }
	}
	
	bool load_player(const string& filename, entity& player) {
	    ifstream infile(filename, ios::binary);
	    if (!infile) return false;
	
	    uint32_t size;
	    if (infile.read((char*)&size, 4)) {
	        vector<uint8_t> block(size);
	        infile.read((char*)block.data(), size);
	        player.unpack(block);
	        return true;
	    }
	    return false;
	}
}
