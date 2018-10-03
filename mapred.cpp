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
#include <math.h> 

using namespace std;

const char* sharedMem = "memory";

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
	//cout << "*vec_itr: " << *vec_itr << '\n';        
        multimap <string, int> :: iterator map_itr = map->find(*vec_itr);
        if ( map_itr == map->end() ) {
            map->insert(make_pair(*vec_itr, 1));
        }
        else {
            map_itr->second = map_itr->second + 1;
        }
    }

    return map;
}

multimap <string, int>* wordShuffle(multimap <string, int> *ptr[], int num_maps, int num_reduces)
{       
     int i = 0; int part_size = 0;
     char* partition = (char*)malloc(sizeof(char)*(num_reduces - 1));
     multimap <string, int> b1;
     multimap <string, int> :: iterator itr;
    
     int split_val = 0;
     if(num_reduces != 1) {
     	split_val = floor(26 / (num_reduces - 1));
        for(i = 0; i < (num_reduces - 1); i++)
        {
  	      if(i == 0)
          {  
            partition[i] = 'a' + split_val;
          }
          else
          {
            partition[i] = partition[i -1] + split_val;
          }
        } part_size = i;
    }

      multimap <string, int> *toReduce = new multimap <string, int> [num_reduces];
      multimap <string, int> current;
      multimap <string, int> :: iterator traverse;
      int index = 0; i = 0;
      for(i = 0; i < num_maps; i++){
        current = *ptr[i]; 
        for (traverse = current.begin(); traverse != current.end(); traverse++) 
        { 
             string word = traverse -> first;
             int j;
             for(j = 0; j < part_size; j++)
             {
                  if(num_reduces == 1)
                  {
                       index = 0;
                       break;
                  }
                  else if((num_reduces == 2) && (tolower(word.at(0)) < 'n'))
                  {
                       index = 0;
                       break;
                  }
                  else if((num_reduces == 2) && (tolower(word.at(0)) >= 'n'))
                  {
                       index = 1; 
                       break;
                  }
                  else if(tolower(word.at(0)) < partition[j])
                  {
                       index = j;
                       break;
                  }
                  else if((j == part_size - 1)||(tolower(word.at(0)) >= partition[part_size - 1]))
                  {
                       index = num_reduces - 1;
                       break;
                  }
             }
             *toReduce[index].insert(make_pair <string, int> (traverse -> first, traverse -> second));
        } 
     } free(partition);
     return toReduce;
}

void* mapIntegerSort(void * input)
{
	string inString = *reinterpret_cast<string*>(input);
	cout << inString + "\n\n";
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
		multimap<string, int>* shuffledMap = new multimap<string, int>[num_reduces];
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

		shuffledMap = wordShuffle(returnValues, num_maps, num_reduces);

		multimap <string, int> :: iterator itr2;
    	for(itr2 = shuffledMap[2].begin(); itr2 != shuffledMap[2].end(); itr2++){
        	cout << itr2->first << '\t' << itr2->second << '\n';
    	}
	}
	//proccesses
	else
	{

	}
}