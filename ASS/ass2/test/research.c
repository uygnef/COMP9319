#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <stdlib.h>


const int BLOCK_BYTES = 10;
const int MAX_NUM_OF_INDEX = 1000;

using namespace std;

map<char, int> count_map;


int occ(char ch, int num, fstream& bwt_file, fstream& index_file);
void get_result(string pattern, string input_file, string _index_file);

int main(){
	string pattern = "ion";
	get_result(pattern, "input.bwt", "test.txt");

}


void get_result(string pattern, string input_file, string _index_file) {

    int j = pattern.size();
    char c = pattern[j];
    int first = count_map[c] + 1;
    int last = count_map[c+1];
//load index file
    fstream index_file;
    index_file.open(_index_file);

    fstream bwt_file;
    bwt_file.open(input_file);
    vector<string> result; //list to store full string(from [] to end)

    while (j >= 1 && first<last) {
        c = pattern[j-1];
        first = count_map[c] + occ(c, first-1, bwt_file, index_file) + 1;
        last = count_map[c] + occ(c, last, bwt_file, index_file);
        j--;
        if(last<first)
            return NULL;
    }
	cout<<"first is"<<first<<"last is"<<last<<endl;
        string full_word;
//backward search find first part
        char next_ch;
        char pre_ch;
        int pre_ch_num;
        int next_ch_num;
        while(pre_ch != '[') {
    
            pre_ch_num = count_map[pre_ch] + occ(c, pre_ch_num-1, bwt_file, index_file) + 1;
            bwt_file.seekg(pre_ch_num,ios::beg);
            bwt_file >> noskipws >> pre_ch;
            full_word = pre_ch + full_word;
        }
//forward search find last part
        while(next_ch != '['){
            next_ch_num = count_map[next_ch] - occ(next_ch, first - 1, bwt_file, index_file) + 1;
            bwt_file.seekg(next_ch_num,ios::beg);
            bwt_file >> noskipws >> next_ch;
            full_word += next_ch;
        }
        result[i] = full_word;
    }
    bwt_file.close();
}

int occ(char ch, int num, fstream& bwt_file, fstream& index_file){

    int line_num = num / BLOCK_BYTES;
    int i = 0;
   //loop to the target line
    string temp;
    while(i < line_num - 1){
        getline(index_file, temp);
        i++;
        //weak constrain, need to fix
        //TODO: fix it
        if(index_file.eof()){
            printf("NUM OUT OF INDEX");
            return -1;
        }
    }

    int key, value;
    map<char, int> last_index;

//get nearest index bucket
    while(1){
        index_file>>key>>value;
        if(key == (int)ch)
            break;
    }

    int extra_num = num % BLOCK_BYTES;
    bwt_file.seekg(line_num*BLOCK_BYTES,ios::beg);
    char bwt_ch;
    for(int i=0; i<extra_num; ++i){

        bwt_file>>noskipws>>bwt_ch;
        if(bwt_ch == ch)
            value++;
        if(bwt_ch == EOF)//TODO: in case out of index
            break;
    }
    return value;
}
