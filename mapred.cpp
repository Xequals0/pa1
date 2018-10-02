#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include <list>
#include <map>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>

using namespace std;

string* split(int numMaps, ifstream& file)
{
	string* out = new string[numMaps];
	string word;
	int i = 0;
	
	while(file >> word)
	{
		out[i % numMaps] += word + " ";
		i++;
	}

	return out;
}

void* mapWordCount(void * input)
{
	string s = *reinterpret_cast<string*>(input);
	multimap<string, int>* map = new multimap<string, int>;
    
	vector<string> parts;
    trim_if(s, boost::is_any_of(" .,;:!-"));
    boost::split(parts, s, boost::is_any_of(" .,;:!-"),boost::token_compress_on);

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
    
    /*
    multimap <string, int> :: iterator itr2;
    for(itr2 = map->begin(); itr2 != map->end(); itr2++){
        cout << itr2->first << '\t' << itr2->second << '\n';
    }
    */
    
    return (void*)map;
}

void* mapIntegerSort(void * input)
{
	string s = *reinterpret_cast<string*>(input);
	cout << s + "\n\n";
    
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
    
   /* multimap <int, int> :: iterator itr2;
    for(itr2 = map->begin(); itr2 != map->end(); itr2++){
        cout << itr2->first << '\t' << itr2->second << '\n';
    }
    */
    
    return (void *)map;
}

int main(int argc, const char* argv[])
{
	void* (*app)(void*);
	char impl[8];
	char* input_file;
	char* output_file;
	int num_maps = 0;
	int num_reduces = 0;
	int i;

	for(i=1; i < argc; i++)
	{
		if(strcmp(argv[i], "--app") == 0)
		{
			i++;
			if(strcmp(argv[i], "wordcount") == 0)
			{
				app = &mapWordCount;
			}
			else if(strcmp(argv[i], "sort") == 0)
			{
				app = &mapIntegerSort; 
			}
			else
			{
				printf("Invalid app\n");
				return -1;
			}
		}
		else if(strcmp(argv[i], "--impl") == 0)
		{
			i++;
			if(!(strcmp(argv[i], "procs") == 0 || strcmp(argv[i], "threads") == 0))
			{
				printf("Invalid implementation\n");
				return -1;
			}
			strcpy(impl, argv[i]);
		}
		else if(strcmp(argv[i], "--maps") == 0)
		{
			i++;
			num_maps = atoi(argv[i]);
			if(num_maps <= 0)
			{
				printf("Invalid number of maps\n");
				return -1;
			}
		}
		else if(strcmp(argv[i], "--reduces") == 0)
		{
			i++;
			num_reduces = atoi(argv[i]);
			if(num_reduces <= 0)
			{
				printf("Invalid number of reduces\n");
				return -1;
			}
		}
		else if(strcmp(argv[i], "--input") == 0)
		{
			i++;
			if(strcmp(argv[i], "") == 0)
			{
				printf("Invalid input file\n");
				return -1;
			}
			input_file = (char*)malloc(strlen(argv[i]) + 1);
			strcpy(input_file, argv[i]);
		}
		else if(strcmp(argv[i], "--output") == 0)
		{
			i++;
			if(strcmp(argv[i], "") == 0)
			{
				printf("Invalid output file\n");
				return -1;
			}
			output_file = (char*)malloc(strlen(argv[i]) + 1);
			strcpy(output_file, argv[i]);
		}
	}

	printf("%s:%d:%d:%s:%s\n", impl, num_maps, num_reduces, input_file, output_file);

	ifstream file(input_file);
	string* splitStrings = split(num_maps, file);

	//threads
	if(strcmp(impl, "threads") == 0)
	{
		pthread_t mapThreads[num_maps];
		multimap<string, int>** returnValues = new multimap<string, int>*[num_maps];
		int i;
		int ret;

		for(i = 0; i < num_maps; i++)
		{
			ret = pthread_create(&mapThreads[i], NULL, app, (void*)&splitStrings[i]);
		}

		for(i = 0; i < num_maps; i++)
		{
			pthread_join(mapThreads[i], (void **)&returnValues[i]);
		}

    	multimap <string, int> :: iterator itr2;
    	for(itr2 = returnValues[0]->begin(); itr2 != returnValues[0]->end(); itr2++){
        	cout << itr2->first << '\t' << itr2->second << '\n';
    	}		
	}
	//proccesses
	else
	{

	}
}
