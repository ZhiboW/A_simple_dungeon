#!/usr/bin/env python3
"""Compiler-extractor for items, drop item list csv to build item database, drop item database to extract component csvs"""

import sys
import struct
import re
import csv
import os
from io import StringIO


#string to binary
def push_string_bin(s):
    b = s.encode("utf-8")
    return struct.pack("I", len(b)) + b

#binary to string
def read_string_bin(b, offset):
    length = struct.unpack_from("I", b, offset)[0]
    print(f"strlen {length}")
    offset += 4
    s = b[offset:offset+length].decode('utf-8')
    offset += length
    return s, offset
#extract csv sections
def split_item_list(filename):
    with open(filename) as infile:
        buffer = infile.read()
    
    blocks = re.split(r"\n(?:\s*,\s*)*\n+", buffer.strip())
    
    return blocks
#extract definitions dictionaries
def get_stat_dict(filename):
    blocks = split_item_list(filename)
    
    statdicts = {}
    
    for block in blocks:
        block = block.strip()
        
        rows = list(csv.reader(StringIO(block)))
        if len(rows) > 2:
            raise IndexError("Bad block size")
        
        tab_name = rows[0][0]
        rkey = rows[0][1:]
        rval = []
        
        for idx, i in enumerate(rows[1][1:]):
            try:
                rval.append(int(i))
            except ValueError:
                raise ValueError("Invalid {} definition {} at index {}".format(tab_name, i, idx))
        
        statdicts[tab_name] = dict(zip(rkey, rval)) #{tabname, {table}}
    
    return statdicts 

class item:
    """item formatting"""
    def __init__(self):
        self.name = ""
        self.id = 0
        self.type = 0
        self.cost = 0
        self.sell = 0
        self.uses = 1
        self.statblock = {}
    
    def from_string(self, block, statdicts):
        rows = list(csv.reader(StringIO(block)))
        
        self.name, id, itype = rows[0]
        
        try:
            self.id = int(id)
        except ValueError:
            raise ValueError("Invalid item id {}".format(id))
        
        try:
            self.type = int(itype)
        except ValueError:
            try:
                self.type = int(statdicts["itemtype"][itype])
            except KeyError:
                raise ValueError("Invalid itemtype for item id {}".format(self.id))
        
        for row in rows[1:]:
            statkey = row[0].lower()
            statval = row[1]
            
            if statkey == "cost":
                try:
                    self.cost = int(statval)
                except ValueError:
                    raise ValueError("Invalid item cost for item id {}".format(self.id))
            elif statkey == "sell":
                try:
                    self.sell = int(statval)
                except ValueError:
                    raise ValueError("Invalid item sell for item id {}".format(self.id))
            elif statkey == "uses":
                try:
                    self.uses = int(statval)
                except ValueError:
                    raise ValueError("Invalid item sell for item id {}".format(self.id))
            else:
                try:
                    statkey = int(statkey)
                except ValueError:
                    try:
                        statkey = statdicts["stat"][statkey]
                    except KeyError:
                        raise ValueError("Invalid stat {} for item id {}".format(statkey, self.id))
                
                try:
                    statval = float(statval)
                except ValueError:
                    raise ValueError("Invalid stat value for {} in item id {}".format(statkey, self.id))
                
                self.statblock[statkey] = statval
    
    def to_string(self):
        buffer = "{},{},{}\ncost,{}\nsell,{}\nuses,{}\n".format(self.name, self.id, self.type, self.cost, self.sell, self.uses)
        
        for key, value in self.statblock.items():
            buffer += "{},{}\n".format(key, value)
        
        return buffer

    def to_bin(self):
        """item length, id, name length, name, cost, sell, stat dict length, stat dict"""
        bin_name = push_string_bin(self.name)
        
        stat_data = bytearray()
        for s_id, s_val in self.statblock.items():
            stat_data.extend(struct.pack("<Id", s_id, s_val))
        
        itemblock = struct.pack("I", self.id) + bin_name + struct.pack("<iiiI", self.cost, self.sell, self.uses, len(self.statblock)) + stat_data
        
        return struct.pack("I", len(itemblock)) + itemblock
    
    def from_bin(self, itemblock, offset):
        blocksize = struct.unpack_from("I", itemblock, offset)[0]
        offset += 4
        self.id = struct.unpack_from("I", itemblock, offset)[0]
        offset += 4
        self.name, offset = read_string_bin(itemblock, offset)
        self.cost, self.sell, self.uses, nstats = struct.unpack_from("<iiiI", itemblock, offset)
        offset += 16
        
        statblock = {}
        for i in range(nstats):
            s_id, s_val = struct.unpack_from("<Id", itemblock, offset)
            offset += 12
            statblock[s_id] = s_val
        self.statblock = statblock
        
        return offset
        
#compile and write items to database
def write_itemdb(statdicts, item_dict, ofname):
    """metadata, ntypes, length of each type, [typeid, nitems, itemdata]"""
    #metadata collection
    outbin = bytearray()
    outbin.extend(struct.pack("I", len(statdicts))) #n tables
    for tablename, table in statdicts.items():
        outbin.extend(struct.pack("I", len(table))) #table length
        outbin.extend(push_string_bin(tablename)) #table name
        for k, v in table.items():
            outbin.extend(push_string_bin(k)) #string
            outbin.extend(struct.pack("I",v)) #int
    #item byte arrays
    type_ids = sorted(item_dict.keys())
    type_sizes = []
    type_bytes = []
    type_counts = []
    for i in type_ids:
        buffer = bytearray()
        for j in item_dict[i]:
            buffer += j.to_bin()
        type_bytes.append(buffer)
        type_sizes.append(len(buffer))
        type_counts.append(len(item_dict[i]))
    #ntypes, length of each type, id list
    outbin.extend(struct.pack("I", len(type_ids)))
    for i in type_ids:
        outbin += struct.pack("I", len(item_dict[i]))
        for j in item_dict[i]:
            outbin += struct.pack("I", j.id)
    #write typeid, size of block, items
    for i in range(len(type_ids)):
        outbin += struct.pack("<II", type_ids[i], len(type_bytes[i])) + type_bytes[i]
    
    with open(ofname, "wb") as outfile:
        outfile.write(outbin)
    return outbin
#extract items from db
def extract_items(ifname):
    with open(ifname, "rb") as infile:
        inbin = infile.read()
    #pull metadata
    offset = 0
    ntables = struct.unpack_from("I", inbin, offset)[0]
    offset += 4
    statdicts = {}
    for i in range(ntables):
        nelem = struct.unpack_from("I", inbin, offset)[0]
        offset += 4
        tablename, offset = read_string_bin(inbin, offset)
        sdblock = {}
        for j in range(nelem):
            name, offset = read_string_bin(inbin, offset)
            val = struct.unpack_from("I", inbin, offset)[0]
            offset += 4
            sdblock[name] = val
        statdicts[tablename] = sdblock
    print(statdicts)
    #ntypes and number of members
    ntypes = struct.unpack_from("I", inbin, offset)[0]
    offset += 4
    itemcounts = []
    for i in range(ntypes):
        itemcount = struct.unpack_from("I", inbin, offset)[0]
        itemcounts.append(itemcount)
        offset += 4
        idlist = []
        for j in range(itemcount):
            idlist.append(struct.unpack_from("I", inbin, offset)[0])
            offset += 4
    item_dict = {}
    for i in range(ntypes):
        print(f"type {i}")
        itype, typesize = struct.unpack_from("<II", inbin, offset)
        offset += 8
        for j in range(itemcounts[i]):
            print(f"itemcount {j} in {itemcounts[i]}")
            buffer = item()
            offset = buffer.from_bin(inbin, offset)
            item_dict.setdefault(buffer.type, []).append(buffer) #dict{type:[items]}
            print(buffer.statblock)
    return item_dict, statdicts
#write items to file
def write_itemcsvs(item_dict, statdicts, metaname, listname):
    with open(metaname, "w", newline = "") as outfile:
        writer = csv.writer(outfile)
        for name, table in statdicts.items():
            writer.writerow([name+"id"] + list(table.keys()))
            writer.writerow([name] + list(table.values()))
            writer.writerow([])
    
    with open(listname, "w") as outfile:
        outfile.write("NAME,ID,TYPE\n")
        outfile.write("STAT,VALUE\n")
        
        for typeid, items in item_dict.items():
            for i in items:
                outfile.write("\n")
                outfile.write(i.to_string())


if __name__ == "__main__":
    if len(sys.argv) < 2: #ask for files
        print("*.db to extract an itemlist, item list and stat block csvs to compile")
    elif sys.argv[1].endswith(".db"): #extract
        item_dict, statdicts = extract_items(sys.argv[1])
        write_itemcsvs(item_dict, statdicts, "r_isb.csv", "r_ilist.csv")
    else:
        statfile = sys.argv[1]
        itemfile = sys.argv[2]
        try:
            statdicts = get_stat_dict(statfile)
        except IndexError:
            statdicts = get_stat_dict(itemfile)
            itemfile = statfile
        item_list = split_item_list(itemfile)
        #collect items
        item_dict = {}
        for block in item_list[1:]:
            buffer = item()
            buffer.from_string(block, statdicts)
            item_dict.setdefault(buffer.type, []).append(buffer) #dict{type:[items]}
        write_itemdb(statdicts, item_dict, "items.db")