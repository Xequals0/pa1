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
#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

using namespace std;

const char* mem = "memory";
const char* sema = "semaphore";
const int SIZE = 100000;
sem_t mutex;
sem_t * sem;
int threads = 0;
int offset = 0;

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

	stringstream ss;
   	boost::archive::text_oarchive oarch(ss);
   	oarch << map;
   	string temp = ss.str();
   	const char* ctemp = temp.c_str();
   	int size = strlen(ctemp)+1;

   	if(threads)
   	{
    	sem_wait(&mutex);
    	int shm_fd = shm_open(mem, O_RDWR, 0666);

   		char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
   		ptr += offset;
    	memcpy(ptr, &size, sizeof(size_t));
    	strcpy(ptr + sizeof(size_t), ctemp);

    	offset += sizeof(size_t) + size;
    	sem_post(&mutex);
   	}
   	else
   	{
   		sem_wait(sem);
    	int shm_fd = shm_open(mem, O_RDWR, 0666);
   		char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
   		int off;
   		memcpy(&off, ptr, sizeof(int));
   		int temp = off + sizeof(size_t) + size;
   		memcpy(ptr, &temp, sizeof(int));
   		ptr += sizeof(int) + off;
    	memcpy(ptr, &size, sizeof(size_t));
    	strcpy(ptr + sizeof(size_t), ctemp);
   		sem_post(sem);
   	}
    return NULL;
}

void* reduceWordCount(void * input)
{
	string s = *reinterpret_cast<string*>(input);
	multimap<string, int>* map = new multimap<string, int>;

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

multimap <int, int>* sortShuffle(multimap <int, int> *ptr[], int num_maps, int num_reduces)
{
     int i = 0; int part_size = 0;
     int min = 0; int max = 0;
     int t1 = 0;
     int* partition = (int*)malloc(sizeof(int)*(num_reduces - 1));
     multimap <int, int> a1;
     multimap <int, int> :: iterator itr;

     /*for(i = 0; i < num_maps; i++){
        a1 = *ptr[i]; 
        for (itr = a1.begin(); itr != a1.end(); itr++) 
        { 
             cout  <<  '\t' << itr->first  
                   <<  '\t' << itr->second << '\n'; 
        } 
        cout << endl;
     }*/


     for(i = 0; i < num_maps; i++){
        a1 = *ptr[i];
        for (itr = a1.begin(); itr != a1.end(); itr++) 
          {
                  t1 = itr-> first;
                  if(min > t1){
                      min = t1;
                  }
                  if(max < t1){
                      max = t1;
                  }
            }
       }
    
     int split_val = 0;
     if(num_reduces == 1) {
     
     }
     else
      {
          split_val = floor((max - min)/(num_reduces - 1));
          //cout << "split: " << max << ',' << min << ',' << split_val << '\n';
          for(i = 0; i < (num_reduces - 1); i++)
          {
              if(i == 0)
              {  
                   partition[i] = min + split_val;
              }
              else
              {
                   partition[i] = partition[i -1] + split_val;
              }
          } part_size = i;
      }

      multimap <int, int> *toReduce = new multimap <int, int> [num_reduces];
      multimap <int, int> current;
      multimap <int, int> :: iterator traverse;
      int index = 0; i = 0;
      for(i = 0; i < num_maps; i++){
        current = *ptr[i]; 
        for (traverse = current.begin(); traverse != current.end(); traverse++) 
        { 
             int number = traverse -> first;
             int j;
             for(j = 0; j < part_size; j++)
             {
                  if(num_reduces == 1)
                  {
                       index = 0;
                       break;
                  }
                  else if((num_reduces == 2) && (number < ((max + min)/2)))
                  {
                       index = 0;
                       break;
                  }
                  else if((num_reduces == 2) && (number >= ((max + min)/2)))
                  {
                       index = 1; 
                       break;
                  }
                  else if(number < partition[j])
                  {
                       index = j;
                       break;
                  }
                  else if(number >= partition[part_size - 1])
                  {
                       index = num_reduces - 1;
                       break;
                  }
             }
             *toReduce[index].insert(make_pair <int, int> (traverse -> first, traverse -> second));
        } 
     } free(partition);
     return toReduce;
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
    
    stringstream ss;
    boost::archive::text_oarchive oarch(ss);
    oarch << map;
    string temp = ss.str();
    const char* ctemp = temp.c_str();
    int size = strlen(ctemp)+1;
    
    if(threads)
    {
        sem_wait(&mutex);
        int shm_fd = shm_open(mem, O_RDWR, 0666);
        
        char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        ptr += offset;
        memcpy(ptr, &size, sizeof(size_t));
        strcpy(ptr + sizeof(size_t), ctemp);
        
        offset += sizeof(size_t) + size;
        sem_post(&mutex);
    }
    else
    {
        sem_wait(sem);
        int shm_fd = shm_open(mem, O_RDWR, 0666);
        char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        int off;
        memcpy(&off, ptr, sizeof(int));
        int temp = off + sizeof(size_t) + size;
        memcpy(ptr, &temp, sizeof(int));
        ptr += sizeof(int) + off;
        memcpy(ptr, &size, sizeof(size_t));
        strcpy(ptr + sizeof(size_t), ctemp);
        sem_post(sem);
    }
    return NULL;
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
		threads = 1;
		pthread_t mapThreads[num_maps];
		multimap<string, int>** returnValues = new multimap<string, int>*[num_maps];
		multimap<string, int>* shuffledMap = new multimap<string, int>[num_reduces];
		int ret;
                
    	int shm_fd = shm_open(mem, O_CREAT | O_RDWR, 0666);
    	ftruncate(shm_fd, SIZE);

    	sem_init(&mutex, 0, 1); 

		for(i = 0; i < num_maps; i++)
		{
			ret = pthread_create(&mapThreads[i], NULL, app, (void*)&splitStrings[i]);
		}

		for(i = 0; i < num_maps; i++)
		{
			pthread_join(mapThreads[i], NULL);
		}

   		char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		for(i = 0; i < num_maps; i++)
		{
    		int len;
    		memcpy(&len, ptr, sizeof(size_t));
    		ptr += sizeof(size_t);
    		char* ctemp = (char*)malloc(len);
    		strcpy(ctemp, ptr);
    		ptr += strlen(ctemp) + 1;
   			stringstream ss;
   			ss << ctemp;
   			boost::archive::text_iarchive iarch(ss);
   			iarch >> returnValues[i];

   			free(ctemp);
		}
		//shuffledMap = wordShuffle(returnValues, num_maps, num_reduces);

		/*multimap <string, int> :: iterator itr2;
    	for(itr2 = shuffledMap[1].begin(); itr2 != shuffledMap[1].end(); itr2++){
        	cout << itr2->first << '\t' << itr2->second << '\n';
    	}*/
		multimap <string, int> :: iterator itr3;
    	for(itr3= returnValues[0]->begin(); itr3 != returnValues[0]->end(); itr3++){
        	cout << itr3->first << '\t' << itr3->second << '\n';
    	}

    	shm_unlink(mem);
    	sem_destroy(&mutex);
	}
	//proccesses
	else
	{
		multimap<string, int>** returnValues = new multimap<string, int>*[num_maps];

		int shm_fd = shm_open(mem, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

 		char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		int temp = 0;
 		memcpy(ptr, &temp, sizeof(int));

		sem = sem_open(sema, O_CREAT | O_RDWR, 0644, 1); 

  	pid_t pid;

		for(i = 0; i < num_maps; i++)
		{
			pid = fork();
			if(pid == 0)
			{
				mapWordCount((void*)&splitStrings[i]);
				exit(0);
			}
			else continue;
		}

		while(wait(NULL) > 0);

 		ptr += sizeof(int);
		for(i = 0; i < num_maps; i++)
		{
    		int len;
    		memcpy(&len, ptr, sizeof(size_t));
    		ptr += sizeof(size_t);
    		char* ctemp = (char*)malloc(len);
    		strcpy(ctemp, ptr);
    		ptr += strlen(ctemp) + 1;
   			stringstream ss;
   			ss << ctemp;
   			boost::archive::text_iarchive iarch(ss);
   			iarch >> returnValues[i];

   			free(ctemp);
		}

		multimap <string, int> :: iterator itr3;
    for(itr3= returnValues[0]->begin(); itr3 != returnValues[0]->end(); itr3++){
      	cout << itr3->first << '\t' << itr3->second << '\n';
    }
    	
    	sem_close(sem);
    	shm_unlink(mem);

	}

	return 0;
}
