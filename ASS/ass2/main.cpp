#include <iostream>
#include <fstream>
#include <cstring>
#include <map>

const int K_value = 10;
const int MAX_NUM_OF_INDEX = 1000;

using namespace std;
/*
 * 怎么生成index，需要n^2的空间。至少2n。
 *
 *
 */

map<char, int> build_index(string input_file,string index_file);
map<char, int> occ_counter;
map<char, int> get_C_from_index_file(string index_file);

int main(int argc, char* argv[]) {

    /* initialized */
    string input_file = argv[1], index_file = argv[2];
    if (argc <= 3)
        return -1;
    string pattern[argc-3];
    for(int i=2; i<argc; i++)
        pattern[i-2] = argv[i];
    fstream  _file;
    _file.open(index_file,ios::in);
    map<char, int> count_map;
    if(!_file)
        count_map = build_index(input_file, index_file);
    else
        count_map = get_C_from_index_file(index_file);
    
    for( map<char,int>::iterator it = count_map.begin(); it != count_map.end(); ++it)
        cout<<"key: "<<it->first <<" value: "<<it->second<<endl;

    return 0;
}


map<char, int> build_index(string input_file, string output_file){
    ifstream my_file;
    ofstream index_file;
    my_file.open(input_file);
    index_file.open(output_file);

    map<char, int> count_list;
    char a;
    int i=0, j=0;
    while(my_file.get(a)){
        count_list[a]++;
        occ_counter[a] += 1;
        i++;

        if(i>K_value){
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
    for( map<char,int>::iterator it = count_list.begin(); it != count_list.end(); ++it){
        temp = it->second;
        it->second = pre_end;
        pre_end += temp;
        index_file<<int(it->first)<<" "<<it->second<<" ";
    }

    return count_list;
}




map<char, int> get_C_from_index_file(string index_file) //copy from http://stackoverflow.com/questions/11876290/c-fastest-way-to-read-only-last-line-of-text-file
{

    ifstream fin;
    fin.open(index_file);
    map<char, int> C;

    
    fin.seekg(-1,ios_base::end);                // go to one spot before the EOF

    bool keepLooping = true;
    while(keepLooping) {
        char ch;
        fin.get(ch);                            // Get current byte's data

        if((int)fin.tellg() <= 1) {             // If the data was at or before the 0th byte
            fin.seekg(0);                       // The first line is the last line
            keepLooping = false;                // So stop there
        }
        else if(ch == '\n') {                   // If the data was a newline
            keepLooping = false;                // Stop at the current position.
        }
        else {                                  // If the data was neither a newline nor at the 0 byte
            fin.seekg(-2,ios_base::cur);        // Move to the front of that data, then to the front of the data before it
        }
    }

    int value;
    int key;
    while(!fin.eof()){
        fin>>key>>value;
        C.insert(make_pair(static_cast<char>(key), value));
    }
    
    fin.close();
    return C;
}

