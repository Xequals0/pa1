
#include "mapred.h"

const char* mem = "memory";
const char* sema = "semaphore";
const int SIZE = 100000;
sem_t mutex;
sem_t * sem;
int threads = 0;
int wordcount = 0;
int offset = 0;

struct comp {
    bool operator() (string s1, string s2) const
    {
  const char* a = s1.c_str();
  const char* b = s2.c_str();  
        int val = strcmp(a, b);
        if(val <= 0)
            return true;
        return false;
        
    }
};

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
        string lower = boost::algorithm::to_lower_copy(*vec_itr);
        multimap <string, int> :: iterator map_itr = map->find(lower);
        if ( map_itr == map->end() ) {
            map->insert(make_pair(lower, 1));
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
    	memcpy(ptr, &size, sizeof(int));
    	strcpy(ptr + sizeof(int), ctemp);

    	offset += sizeof(int) + size;
    	sem_post(&mutex);
   	}
   	else
   	{
   		sem_wait(sem);
    	int shm_fd = shm_open(mem, O_RDWR, 0666);
   		char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
   		int off;
   		memcpy(&off, ptr, sizeof(int));
   		int temp = off + sizeof(int) + size;
   		memcpy(ptr, &temp, sizeof(int));
   		ptr += sizeof(int) + off;
    	memcpy(ptr, &size, sizeof(int));
    	strcpy(ptr + sizeof(int), ctemp);
   		sem_post(sem);
   	}
    return NULL;
}

void* reduceWordCount(void * input)
{
	multimap<string, int>* inmap = reinterpret_cast<multimap<string,int>*>(input);
  map<string, int>* outmap = new map<string,int>;
  multimap<string, int> :: iterator itr;

  for(itr = inmap->begin(); itr != inmap->end(); itr++)
  {
    map <string, int> :: iterator map_itr = outmap->find(itr->first);
    if ( map_itr == outmap->end() ) {
      outmap->insert(make_pair(itr->first, itr->second));
    }
    else {
      map_itr->second = map_itr->second + itr->second;
    }
  }

  stringstream ss;
  boost::archive::text_oarchive oarch(ss);
  oarch << outmap;
  string temp = ss.str();
  const char* ctemp = temp.c_str();
  int size = strlen(ctemp)+1;

  if(threads)
  {
    sem_wait(&mutex);
    int shm_fd = shm_open(mem, O_RDWR, 0666);

    char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    ptr += offset;
    memcpy(ptr, &size, sizeof(int));
    strcpy(ptr + sizeof(int), ctemp);

    offset += sizeof(int) + size;
    sem_post(&mutex);
  }
  else
  {
    sem_wait(sem);
    int shm_fd = shm_open(mem, O_RDWR, 0666);
    char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    int off;
    memcpy(&off, ptr, sizeof(int));
    int temp = off + sizeof(int) + size;
    memcpy(ptr, &temp, sizeof(int));
    ptr += sizeof(int) + off;
    memcpy(ptr, &size, sizeof(int));
    strcpy(ptr + sizeof(int), ctemp);
    sem_post(sem);
  }

  return NULL;
}

void* reduceIntegerSort(void * input)
{
    multimap<int, int>* inmap = reinterpret_cast<multimap<int,int>*>(input);
    map<int, int>* outmap = new map<int,int>;
    multimap<int, int> :: iterator itr;
    
    for(itr = inmap->begin(); itr != inmap->end(); itr++)
    {
        map <int, int> :: iterator map_itr = outmap->find(itr->first);
        if ( map_itr == outmap->end() ) {
            outmap->insert(make_pair(itr->first, itr->second));
        }
        else {
            map_itr->second = map_itr->second + itr->second;
        }
    }
    
    stringstream ss;
    boost::archive::text_oarchive oarch(ss);
    oarch << outmap;
    string temp = ss.str();
    const char* ctemp = temp.c_str();
    int size = strlen(ctemp)+1;
    
    if(threads)
    {
        sem_wait(&mutex);
        int shm_fd = shm_open(mem, O_RDWR, 0666);
        
        char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        ptr += offset;
        memcpy(ptr, &size, sizeof(int));
        strcpy(ptr + sizeof(int), ctemp);
        
        offset += sizeof(int) + size;
        sem_post(&mutex);
    }
    else
    {
        sem_wait(sem);
        int shm_fd = shm_open(mem, O_RDWR, 0666);
        char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        int off;
        memcpy(&off, ptr, sizeof(int));
        int temp = off + sizeof(int) + size;
        memcpy(ptr, &temp, sizeof(int));
        ptr += sizeof(int) + off;
        memcpy(ptr, &size, sizeof(int));
        strcpy(ptr + sizeof(int), ctemp);
        sem_post(sem);
    }
    
    return NULL;


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
     if(num_reduces != 1) {
          split_val = floor((max - min)/(num_reduces - 1));
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

void combineAndOutput(void ** inMap, char* output_file, int num_reduces)
{
  if(wordcount)
  {
    map<string, int>** maps = reinterpret_cast<map<string,int>**>(inMap);
    map<string, int, comp> outmap;
    map<string, int> :: iterator itr;

    for(int i = 0; i < num_reduces; i++)
    {
      for(itr = maps[i]->begin(); itr != maps[i]->end(); itr++)
      {
        outmap.insert(make_pair(itr->first,itr->second));
      }
    }

    ofstream outfile;
    outfile.open (output_file);

    for(itr = outmap.begin(); itr != outmap.end(); itr++)
    {
      outfile << itr->first << '\t' << itr->second << '\n';
    }
    
    outfile.close();
  }
  else
  {

  }
} 

int main(int argc, const char* argv[])
{
	void* (*mapapp)(void*);
  void* (*redapp)(void*);
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
				mapapp = &mapWordCount;
        redapp = &reduceWordCount;
        wordcount = 1;
			}
			else if(strcmp(argv[i], "sort") == 0)
			{
				mapapp = &mapIntegerSort;
        redapp = &reduceIntegerSort; 
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
    map<string, int>** reduced = new map<string, int>*[num_reduces];
                
    int shm_fd = shm_open(mem, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

    sem_init(&mutex, 0, 1); 

		for(i = 0; i < num_maps; i++)
		{
			pthread_create(&mapThreads[i], NULL, mapapp, (void*)&splitStrings[i]);
		}

		for(i = 0; i < num_maps; i++)
		{
			pthread_join(mapThreads[i], NULL);
		}

   	char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		for(i = 0; i < num_maps; i++)
		{
    		int len;
    		memcpy(&len, ptr, sizeof(int));
    		ptr += sizeof(int);
    		char* ctemp = (char*)malloc(len);
    		strcpy(ctemp, ptr);
    		ptr += strlen(ctemp) + 1;
   			stringstream ss;
   			ss << ctemp;
   			boost::archive::text_iarchive iarch(ss);
   			iarch >> returnValues[i];

   			free(ctemp);
		}

    offset = 0;

		shuffledMap = wordShuffle(returnValues, num_maps, num_reduces);

    pthread_t reduceThreads[num_reduces];

    for(i = 0; i < num_reduces; i++)
    {
      pthread_create(&reduceThreads[i], NULL, redapp, (void*)&shuffledMap[i]);
    }

    for(i = 0; i < num_reduces; i++)
    {
      pthread_join(reduceThreads[i], NULL);
    }

    ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for(i = 0; i < num_reduces; i++)
    {
        int len;
        memcpy(&len, ptr, sizeof(int));

        ptr += sizeof(int);
        char* ctemp = (char*)malloc(len);
        strcpy(ctemp, ptr);
        ptr += strlen(ctemp) + 1;
        stringstream ss;
        ss << ctemp;
        boost::archive::text_iarchive iarch(ss);
        iarch >> reduced[i];

        free(ctemp);
    }

    combineAndOutput((void**)reduced, output_file, num_reduces);

    shm_unlink(mem);
    sem_destroy(&mutex);
	}
	//proccesses
	else
	{
		multimap<string, int>** returnValues = new multimap<string, int>*[num_maps];
    multimap<string, int>* shuffledMap = new multimap<string, int>[num_reduces];
    map<string, int>** reduced = new map<string, int>*[num_reduces];

		int shm_fd = shm_open(mem, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

 		char* ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		int temp = 0;
 		memcpy(ptr, &temp, sizeof(int));

		sem = sem_open(sema, O_CREAT | O_RDWR, 0644, 0);
    sem_post(sem); 

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
    		memcpy(&len, ptr, sizeof(int));
    		ptr += sizeof(int);
    		char* ctemp = (char*)malloc(len);
    		strcpy(ctemp, ptr);
    		ptr += strlen(ctemp) + 1;
   			stringstream ss;
   			ss << ctemp;
   			boost::archive::text_iarchive iarch(ss);
   			iarch >> returnValues[i];

   			free(ctemp);
		}

    shuffledMap = wordShuffle(returnValues, num_maps, num_reduces);

    ptr = (char*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memcpy(ptr, &temp, sizeof(int));

    for(i = 0; i < num_reduces; i++)
    {
      pid = fork();
      if(pid == 0)
      {
        reduceWordCount((void*)&shuffledMap[i]);
        exit(0);
      }
      else continue;
    }

    while(wait(NULL) > 0);

    ptr += sizeof(int);
    for(i = 0; i < num_reduces; i++)
    {
        int len;
        memcpy(&len, ptr, sizeof(int));
        ptr += sizeof(int);
        char* ctemp = (char*)malloc(len);
        strcpy(ctemp, ptr);
        ptr += strlen(ctemp) + 1;
        stringstream ss;
        ss << ctemp;
        boost::archive::text_iarchive iarch(ss);
        iarch >> reduced[i];

        free(ctemp);
    }

    combineAndOutput((void**)reduced, output_file, num_reduces);
    	
    sem_close(sem);
    shm_unlink(mem);

	}

	return 0;
}
