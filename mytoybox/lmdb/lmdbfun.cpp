#include <cstdio>
#include <cstdlib>
#include <lmdb++.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <unordered_map>
#include <map>

std::ifstream::pos_type filesize(const std::string& filename)
{
    std::ifstream in(filename.c_str(), std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

uint64_t getus() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
}

int access_lmdb(const char* dbdir)
{
  auto env = lmdb::env::create();
  env.set_mapsize(1UL * 1024UL * 1024UL );  
  env.open(dbdir, 0, 0664); // lmdb create data.mdb and lock.mdb under this folder
  auto rtxn = lmdb::txn::begin(env, nullptr, MDB_RDONLY);
  auto dbi = lmdb::dbi::open(rtxn, nullptr);
  auto cursor = lmdb::cursor::open(rtxn, dbi);
  std::string key, value;
  int cnt = 0;
  while (cursor.get(key, value, MDB_NEXT)) {
    if((cnt % (1024*768)) ==0)
      std::printf("key: '%s', value: '%s'\n", key.c_str(), value.c_str());
    cnt++;
  }
  std::printf("key: '%s', value: '%s'\n", key.c_str(), value.c_str());
  cursor.close();
  rtxn.abort();
  return cnt;
}

void create_lmdb(const char* dbdir)
{
  // insert 1 million dummy records into the lmdb file
  auto env = lmdb::env::create();
  env.set_mapsize(1UL * 1024UL * 1024UL*512 );  
  env.open(dbdir, 0, 0664);
  std::cout << __func__ << " add records\n";
  char keybuf[32], valbuf[32];
  int id = 0;
  for(int j=0; j<1024; j++) {
    auto wtxn = lmdb::txn::begin(env);
    auto dbi = lmdb::dbi::open(wtxn, nullptr);
    for(int i=0; i<1024; i++) {
      sprintf(keybuf, "key%06d", id);
      sprintf(valbuf, "value%06d", id); id++;
      dbi.put(wtxn, keybuf, valbuf);
      
    }
    wtxn.commit();
  }
}

int main() {
  const char* dbdir = "/home/onega/src/lmdb/test";  
  std::string dbfile = std::string(dbdir)+"/data.mdb";
  uint64_t start = getus();
  if(filesize(dbfile)<1024*1024) {
     create_lmdb(dbdir);
     std::cout << "create_lmdb spent " << getus()-start << " us\n";
  }
  std::cout << dbfile << " size " << filesize(dbfile) << std::endl;
  start = getus();
  int cnt = access_lmdb(dbdir);
  std::cout << "it took " << getus()-start << " to access " << cnt  << " records\n";
  start = getus();
  std::map<std::string, std::string> map;
  char keybuf[32], valbuf[32];
  int id = 0;
  for(int j=0; j<1024; j++) {
    for(int i=0; i<1024; i++) {
      sprintf(keybuf, "key%06d", id);
      sprintf(valbuf, "value%06d", id); id++;
      map[keybuf] = valbuf;      
    }     
  }
  std::cout << "it took " << getus()-start << " to insert " << map.size()  << " records into std::map\n";
  return EXIT_SUCCESS;
}

/* 
# lmdb
git clone https://github.com/LMDB/lmdb
scl enable devtoolset-3 bash
cd lmdb/libraries/liblmdb
make && make install﻿

[onega@localhost lmdb]$ git clone https://github.com/bendiken/lmdbxx.git
Cloning into 'lmdbxx'...
remote: Counting objects: 569, done.
remote: Total 569 (delta 0), reused 0 (delta 0), pack-reused 569
Receiving objects: 100% (569/569), 111.18 KiB | 0 bytes/s, done.
Resolving deltas: 100% (343/343), done.

[onega@localhost lmdb]$ LD_LIBRARY_PATH=/home/onega/src/lmdb/libraries/liblmdb /home/onega/src/lmdb/test/lmdbfun
create_lmdb add records
create_lmdb spent 8530138 us
/home/onega/src/lmdb/test/data.mdb size 84250624
key: 'key000000', value: 'value000000'
key: 'key737856', value: 'value737856'
key: 'key999999', value: 'value999999'
it took 122721 to access 1048576 records
it took 1747114 to insert 1048576 records into std::map
*/
