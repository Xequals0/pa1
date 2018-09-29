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

using namespace std;


multimap<string, int>*  map_wordCount(string s) {
    
    multimap<string, int>* map = new multimap<string, int>;
    
	vector<string> parts;
	boost::split(parts, s, boost::is_any_of(" .,;:!-"));

	vector<string>::iterator vec_itr;

	for(vec_itr = parts.begin(); vec_itr != parts.end(); vec_itr++){
	       // cout << *vec_itr << '\n';
        
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


int main(int argc, const char * argv[]) {
    // insert code here...
    /*
    multimap<string, int> map;

    //map.insert(pair <string, int> ("banana", 1));
    //map.insert(pair <string, int> ("banana", 2));
    //map.insert(pair <string, int> ("apple", 1));
    //map.insert(pair <string, int> ("cookie", 1));
    
    list < pair <string, int> > list;
    list.push_front(make_pair("donut", 1));
    list.push_front(make_pair("apple", 1));
    list.push_front(make_pair("banana", 1));
    list.push_front(make_pair("cookie", 2));
    list.push_front(make_pair("cookie", 1));
    
    std::list < pair <string, int> >::iterator itr;
    
    for(itr = list.begin(); itr != list.end(); itr++){
        cout << "itr->first: " << itr->first << '\t'
             << "itr->second: " << itr->second << '\n';
        
        multimap <string, int> :: iterator map_itr = map.find(itr->first);
        if ( map_itr == map.end() ) {
            map.insert(make_pair(itr->first, itr->second));
        } else {
            map_itr->second = map_itr->second + itr->second;
        }
    }

    multimap <string, int> :: iterator itr2;
    for(itr2 = map.begin(); itr2 != map.end(); itr2++){
        cout << itr2->first << '\t' << itr2->second << '\n';
    }
    */

	map_wordCount("banana;apple.cookie;apple,apple");
	
    return -1;
}
