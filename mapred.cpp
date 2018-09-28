#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h> 

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
	string inString = *reinterpret_cast<string*>(input);
	cout << inString + "\n\n";
}

int main(int argc, const char* argv[])
{
	char app[10];
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
			if(!(strcmp(argv[i], "wordcount") == 0 || strcmp(argv[i], "sort") == 0))
			{
				printf("Invalid app\n");
				return -1;
			}
			strcpy(app, argv[i]);
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

	printf("%s:%s:%d:%d:%s:%s\n", app, impl, num_maps, num_reduces, input_file, output_file);

	if(strcmp(app,"wordcount") == 0)
	{
		ifstream file(input_file);
		string* splitStrings = split(num_maps, file);

		if(strcmp(impl, "threads") == 0)
		{
			pthread_t mapThreads[num_maps];
			int i;
			int ret;

			for(i = 0; i < num_maps; i++)
			{
				ret = pthread_create(&mapThreads[i], NULL, mapWordCount, (void*)&splitStrings[i]);
			}

			for(i = 0; i < num_maps; i++)
			{
				pthread_join(mapThreads[i], NULL);
			}

		}
	}
}