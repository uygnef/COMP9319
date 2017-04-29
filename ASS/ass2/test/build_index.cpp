#include <iostream>
#include <fstream>
#include <map>
#include <vector>

const int BYTES_OF_BLOCK = 10;
const int MAX_NUM_OF_INDEX = 1000;
using namespace std;

map<char, int> occ_counter;
map<char, int> get_C_from_index_file(string index_file);
map<char, int> count_map;
int build_index(string input_file, string output_file);

int main(int argc, char* argv[]) {

    /*create index file*/

    build_index("input.bwt", "index.file");
    return 0;
}

int build_index(string input_file, string output_file){
    ifstream my_file;
    ofstream index_file;
    my_file.open(input_file);
    index_file.open(output_file);

    char a;
    int i=0, j=0;
    while(my_file.get(a)){
        count_map[a]++;
        occ_counter[a] += 1;
        i++;

        if(i>=BYTES_OF_BLOCK){
/*
 * index_file's format: ASCII number of char  (space)  occ number  .....
 *                              10                          1      .....
 * each row calculate occ number of K*n char in bwt file. Using new line to separate.
 */
            for(map<char,int>::iterator it = occ_counter.begin(); it != occ_counter.end(); ++it)
                index_file<<int(it->first)<<" "<<it->second<<" ";

            index_file<<endl;
            j++;
            i = 0;
        }
    }

    // index is |# a a a b b c d d d f| ------->  a:1, b:4, c:6, d:7, f:10
    int temp=0, pre_end=1;
    for( map<char,int>::iterator it = count_map.begin(); it != count_map.end(); ++it){
        temp = it->second;
        it->second = pre_end;
        pre_end += temp;
        index_file<<int(it->first)<<" "<<it->second<<" ";
    }
    index_file<<127<<" "<<pre_end; //the last char end, like $ in bwt encode.
    return 0;
}