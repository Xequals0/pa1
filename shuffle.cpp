#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream> 
#include <string> 
#include <iterator>
#include <map>
#include <utility>


using namespace std;

//g++ -W -Wall -pedantic -o shuffle shuffle.cpp

multimap <string, int>* wordShuffle(multimap <string, int> *ptr[], int num_maps, int num_reduces)
{       
     int i = 0; int part_size = 0;
     char* partition = (char*)malloc(sizeof(char)*(num_reduces - 1));
     multimap <string, int> b1;
     multimap <string, int> :: iterator itr;
     for(i = 0; i < num_maps; i++){
        b1 = *ptr[i]; 
        for (itr = b1.begin(); itr != b1.end(); itr++) 
        { 
             cout  <<  '\t' << itr->first  
                   <<  '\t' << itr->second << '\n'; 
        } 
        cout << endl;
     }
    
     int split_val = 0;
     if(num_reduces == 1) {
     }/* else if(num_reduces == 2) {
         char partition [] = {'n','\0'};
         part_size = 1;
         cout << "hi: " << partition << '\n';
     } else if(num_reduces == 14)
     {
         char partition [] = {'b','d','f','g','i','k','m','o','q','s','u','w','y','\0'};
         part_size = 13;
     }*/
     else
      {
          
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
      
    /*  *toReduce[1].insert(make_pair <string, int> ("SQL", 2));
      itr = toReduce[1].begin();
           cout << "\n hi" << itr -> first;
           cout  << endl;*/
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
                  //cout << word.at(0) << "," << j << '\n';
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

int main()
{
     int num_maps = 2;
     int num_reduces = 6;
     int i = 0;
     int te = 1;
     multimap <int, int> b1;
     b1.insert(pair <int, int> (96, 1));
     b1.insert(pair <int, int> (950, 1));
     b1.insert(pair <int, int> (424, 3));
     b1.insert(pair <int, int> (te, 4));
     b1.insert(pair <int, int> (1, 1));

     multimap <int, int> c1;
     c1.insert(pair <int, int> (2400, 1));
     c1.insert(pair <int, int> (712, 1));
     c1.insert(pair <int, int> (237, 3));
     c1.insert(pair <int, int> (1800, 4));
     c1.insert(pair <int, int> (1329, 1));
    
     /*multimap <int, int> :: iterator itr;
     for (itr = b1.begin(); itr != b1.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;*/

     multimap <string, int> one;
     one.insert(pair <string, int> ("SQL", 2));
     one.insert(pair <string, int> ("DW", 1));
/*
     for (itr = one.begin(); itr != one.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;
*/
     multimap <string, int> two;
     two.insert(pair <string, int> ("SQL", 1));
     two.insert(pair <string, int> ("SSIS", 1));
     two.insert(pair <string, int> ("SSRS", 1));
/*
     for (itr = two.begin(); itr != two.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;
*/
     multimap <string, int> three;
     three.insert(pair <string, int> ("SQL", 1));
     three.insert(pair <string, int> ("Yarn", 1));
     three.insert(pair <string, int> ("SSRS", 1));/*

     for (itr = three.begin(); itr != three.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;
*/
     multimap <string, int> four;
     four.insert(pair <string, int> ("DW", 1));
     four.insert(pair <string, int> ("BI", 1));
     four.insert(pair <string, int> ("ZANE", 1));
/*
     for (itr = four.begin(); itr != four.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;
*/
     /* multimap <string, int> *m1;
     m1 = &one;
     shuffle(m1);*/
     
     multimap <string, int> *ptr[4];   
     ptr[0] = &one;
     ptr[1] = &two;
     ptr[2] = &three;
     ptr[3] = &four;
    /* multimap <string, int>* fin = wordShuffle(ptr, num_maps, num_reduces);

     
     multimap <string, int> :: iterator temp;
     for(i = 0; i < num_reduces; i++){
        for (temp = fin[i].begin(); temp != fin[i].end(); temp++) 
        { 
             cout  <<  '\t' << temp->first  
                   <<  '\t' << temp->second << '\n'; 
        } cout << "End of Word Bucket: ";
        cout << endl;
     }*/

    multimap <int, int> *arr[1];
    arr[0] = &b1;
    arr[1] = &c1;
    multimap <int, int>* shuffled = sortShuffle(arr, num_maps, num_reduces);
    multimap <int, int> :: iterator int_itr;
    for(i = 0; i < num_reduces; i++){
        for (int_itr = shuffled[i].begin(); int_itr != shuffled[i].end(); int_itr++) 
        { 
             cout  <<  '\t' << int_itr->first  
                   <<  '\t' << int_itr->second << '\n'; 
        } cout << "End of Int Bucket: ";
        cout << endl;
     }

    return 0;
}
