#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

	printf("%s:%s:%d:%d:%s:%s", app, impl, num_maps, num_reduces, input_file, output_file);
}