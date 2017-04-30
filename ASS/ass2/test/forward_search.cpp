#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

#define LAST_CHAR 127
const int BLOCK_BYTES = 10;
const int MAX_NUM_OF_INDEX = 1000;
using namespace std;

map<char, int> occ_counter;
map<char, int> get_C_from_index_file(string index_file);
map<char, int> count_map;
char the_i_th_char(int& i);

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
    printf("***  index is ***\n");
    while(!fin.eof()){
        fin>>key>>value;
        C.insert(make_pair(static_cast<char>(key), value));
        cout<<(char)key<<":"<<value<<"\t";
    }
    printf("\n*** ******* ***\n");
    fin.close();
    return C;
}


int find_i_char(int i, char ch, fstream& bwtfile, fstream& index_file){
    printf("---------find i char : i is %d, ch is %c\n", i, ch);

    int result = 0;
    index_file.clear();
    int key;
    int value=0, line_num=0;
    string line;
    while(value < i){
        if(!getline(index_file, line)){
            line_num++;
            break;
        }
        cout<<line<<endl;
        stringstream stream(line);
        line_num++;
        while(!stream.eof()){
            stream>>key>>value;
            printf("key is %c, value is %d\n", (char)key, value);
            if(key == ch) {
                if (value < i && value>result) {
                    result = value;
                    printf("result: %d\n", result);
                }else{
                    break;
                }
            }
            if(key>=ch)
                break;
            value = 0;
        }
    }
    line_num--;
    int ret = line_num*BLOCK_BYTES;
    printf("ret is %d, result is %d\n", ret, result);

    bwtfile.clear();
    bwtfile.seekg(ret);
    char chr;
    while(result != i && !bwtfile.eof()){
        if(result > i){
            printf("find %d char ERR: OUT OF INDEX %d\n", result, i);
            return -1;
        }
        bwtfile>>noskipws>>chr;
        cout<<chr<<endl;
        if(chr == ch){
            result++;
        }
        ret++;
    }
    return ret;
}

string forward_search(int i, fstream& bwt_file, fstream& index_file){
    printf("----forward_search: i is %d\n", i);
    char  ch = the_i_th_char(i);
    string full_word = ""+ch;
    while(ch != '['){
        full_word += ch;
        i = find_i_char(i, ch, bwt_file, index_file);
        ch = the_i_th_char(i);
    }
    printf("%dth char is %c\n",i, ch);
    return full_word;
}

char the_i_th_char(int& i){
    printf("---------the i the char : i is %d\n", i);
    char ch, temp_ch;
    int temp_value;
    for( map<char,int>::iterator it = count_map.begin(); it != count_map.end(); ++it){
        temp_ch = it->first;
        temp_value = it->second;
        printf("---------temo_ch %c, temp value %d\n", temp_ch, temp_value);
        if(temp_value > i)
            break;
        ch = temp_ch;
    }
    i = i - count_map[ch] + 1;
    printf("---------char is %c\n", ch);
    return ch;
}

int main(int argc, char* argv[]) {
    string arg_2 = argv[2];
    char c = argv[1][0];
    int num = std::stoi(arg_2);
    cout<<"num = "<<num<<endl;
    fstream bwtfile;
    bwtfile.open("input.bwt");
    fstream index_file;
    index_file.open("index.file");
    count_map=get_C_from_index_file("index.file");
    cout<< forward_search(num, bwtfile, index_file);
    return 1;
}