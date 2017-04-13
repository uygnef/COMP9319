#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>


const int BLOCK_BYTES = 1000;
const int MAX_NUM_OF_INDEX = 1000;

using namespace std;


int main(){
   fstream file;
   file.open("test.txt", ios::in);

    int i = 0;
    string line;
    while(i < 3){
        getline(file, line);
        i++;
    }

    int key, pre_key=-1;
    int value;
    map<char, int> C;
	
	while(1){
        file>>key>>value;
		
        if(pre_key > key)
            break;
		pre_key = key;
		cout<<key<<" "<<value<<" ";
        C.insert(make_pair(key, value));	
    }
return 0;
}
