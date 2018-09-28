#include <stdio.h>
#include <stdlib.h>
#include <iostream> 
#include <string> 
#include <iterator>
#include <map>
#include <utility>
using namespace std;

//g++ -W -Wall -pedantic -o shuffle shuffle.cpp

int shuffle(multimap <string, int> *ptr[])
{
     
     multimap <string, int> b1 = *ptr[3];
     multimap <string, int> :: iterator itr;
     for (itr = b1.begin(); itr != b1.end(); itr++) 
     { 
        cout  <<  '\t' << itr->first  
              <<  '\t' << itr->second << '\n'; 
     } 
     cout << endl;
     





     return 0;
}


int main()
{
     string s = "He World";
     cout << s << endl;

     int num_maps = 4;
     int num_reduces = 6;


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
     three.insert(pair <string, int> ("SSAS", 1));
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
     four.insert(pair <string, int> ("SQL", 1));
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
     shuffle(ptr);

     return 0;
}
