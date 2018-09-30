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

int wordShuffle(multimap <string, int> *ptr[], int num_maps, int num_reduces)
{
         
     int i = 0; int part_size = 0;
     char* partition;
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
         char partition [] = {'z'};
         part_size = 1;
     } else if(num_reduces == 2) {
         char partition [] = {'n'};
         part_size = 1;
     } else if(num_reduces == 14)
     {
         char partition [] = {'b','d','f','g','i','k','m','o','q','s','u','w','y'};
         part_size = 13;
     }
     else
      {
          char* partition = new char[num_reduces - 1];
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
              // cout  << "hi\t" << partition[i];
              // cout  << endl;
          } part_size = num_reduces;
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
             int j = 0;

             for(j = 0; j < part_size; j++)
             {
                 cout << partition[j] << '\n';
             }

             for(j = 0; j < part_size; j++)
             {
                  //cout << word.at(0) << "," << partition[j] << '\n';
                  if(word.at(0) < partition[j])
                  {
                       index = j;
                       break;
                  }
                  else if((j == part_size - 1) || (word.at(0) >= partition[part_size - 1]))
                  {
                       index = num_reduces - 1;

/*
                  cout << "hi: " << index << '\n';
                  cout << endl;

*/

                       break;
                  }
                 // cout << index << '\n';
                 // cout << endl;
             }
             *toReduce[index].insert(make_pair <string, int> (traverse -> first, traverse -> second));
        } 
     }
     
     
     multimap <string, int> :: iterator temp;
     for(i = 0; i < num_reduces; i++){
        for (temp = toReduce[i].begin(); temp != toReduce[i].end(); temp++) 
        { 
             cout  <<  '\t' << temp->first  
                   <<  '\t' << temp->second << '\n'; 
        } cout << "next: ";
        cout << endl;
     }



     return 0;
}


int main()
{
     string s = "He World";
     cout << s << endl;

     int num_maps = 4;
     int num_reduces = 14;


     int te = 1;
     multimap <int, int> b1;
     b1.insert(pair <int, int> (96, 1));
     b1.insert(pair <int, int> (950, 1));
     b1.insert(pair <int, int> (424, 3));
     b1.insert(pair <int, int> (te, 4));
     b1.insert(pair <int, int> (1, 1));

     multimap <int, int> :: iterator itr;
     for (itr = b1.begin(); itr != b1.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;

     multimap <string, int> one;
     one.insert(pair <string, int> ("app", 2));
     one.insert(pair <string, int> ("Mon", 1));
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
     three.insert(pair <string, int> ("l", 1));
     three.insert(pair <string, int> ("g", 1));
     three.insert(pair <string, int> ("fee", 1));/*

     for (itr = three.begin(); itr != three.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;
*/
     multimap <string, int> four;
     four.insert(pair <string, int> ("kre", 1));
     four.insert(pair <string, int> ("t", 1));
     four.insert(pair <string, int> ("no", 1));
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
     wordShuffle(ptr, num_maps, num_reduces);

     return 0;
}
