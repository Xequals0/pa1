//
//  main.cpp
//  MapReduce
//
//  Created by Juan Uribe on 9/28/18.
//  Copyright © 2018 JuanUribe. All rights reserved.
//

#include <iostream>
#include <list>
#include <map>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;


void testingSharedMemory{
    const int SIZE = sizeof(multimap<string,int>) + 3 * sizeof(pair<string, int>); //want to enter 3 <string, int> into hashtable
    
    const char *name = "name";
    
    int shm_fd;
    
    multimap<string,int>* ptr;
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    
    ftruncate(shm_fd, SIZE);
    
    ptr = (multimap<string,int>*) mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    //Writing to shared memory object
    
    //I get seg fault here
    ptr->insert(make_pair("a", 1));
    ptr->insert(make_pair("b", 1));
    ptr->insert(make_pair("c", 1));
}

multimap<string, int>*  map_wordCount(string s) {
    
    multimap<string, int>* map = new multimap<string, int>;
    
    vector<string> parts;
    trim_if(s, boost::is_any_of(" .,;:!-"));
    boost::split(parts, s, boost::is_any_of(" .,;:!-"),boost::token_compress_on);

  vector<string>::iterator vec_itr;
    
    for(vec_itr = parts.begin(); vec_itr != parts.end(); vec_itr++){
	//cout << "*vec_itr: " << *vec_itr << '\n';        
        multimap <string, int> :: iterator map_itr = map->find(*vec_itr);
        if ( map_itr == map->end() ) {
            map->insert(make_pair(*vec_itr, 1));
        }
        else {
            map_itr->second = map_itr->second + 1;
        }
    }
    multimap <string, int> :: iterator itr2;
    for(itr2 = map->begin(); itr2 != map->end(); itr2++){
        cout << itr2->first << '\t' << itr2->second << '\n';
    }

    return map ;
}

multimap<int, int>*  map_IntegerSort(string s) {
    
    multimap<int, int>* map = new multimap<int, int>;
    
    vector<string> parts;
    boost::split(parts, s, boost::is_any_of(" .,;:!-"),boost::token_compress_on);    
    vector<string>::iterator vec_itr;
    
    for(vec_itr = parts.begin(); vec_itr != parts.end(); vec_itr++){
        
	int num;
	if ( ! (istringstream(*vec_itr) >> num) ) 
		num = 0;
        
	if((*vec_itr)[0] != '0' && num == 0) continue; 
	
	multimap <int, int> :: iterator map_itr = map->find(num);
        if ( map_itr == map->end() ) {
            map->insert(make_pair(num, 1));
        }
        else {
            map_itr->second = map_itr->second + 1;
        }
    }
    
    multimap <int, int> :: iterator itr2;
    for(itr2 = map->begin(); itr2 != map->end(); itr2++){
        cout << itr2->first << '\t' << itr2->second << '\n';
    }
    
   
    return map;
}

int main(int argc, const char * argv[]) {

    map_IntegerSort("23;18;23...!!!;;1:22.2;0.000000000");
       
    return -1;
}
