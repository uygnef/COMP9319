#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>

const int BLOCK_BYTES = 1000;
const int MAX_NUM_OF_INDEX = 1000;

using namespace std;
/*
 * 怎么生成index，需要n^2的空间。至少2n。
 *
 *
 */

int build_index(string input_file,string index_file);
map<char, int> occ_counter;
map<char, int> get_C_from_index_file(string index_file);
map<char, int> count_map;


vector<string> get_result(string pattern, string input_file, string _index_file);
int search(vector<string> pattern_list, string bwt_file, string index_file);
int occ(char ch, int num, fstream& index_file);


int main(int argc, char* argv[]) {

    /* initialized */
    string input_file = argv[1], index_file = argv[2];
    if (argc <= 3)
        return -1;
    vector<string> pattern(argc-3);
    for(int i=3; i<argc; i++)
        pattern[i-3] = argv[i];

    /*create index file*/
    fstream  _file;
    _file.open(index_file,ios::in);
    if(!_file)                                             //if index file do not exist, build it. otherwise read Count map from it.
        build_index(input_file, index_file);
    else
        count_map = get_C_from_index_file(index_file);

    for( map<char,int>::iterator it = count_map.begin(); it != count_map.end(); ++it)
        cout<<"key: "<<it->first <<" value: "<<it->second<<endl;

    search(pattern, input_file, index_file);

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

        if(i>BLOCK_BYTES){
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

    return 0;
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

int search(vector<string> pattern_list, string bwt_file, string index_file){
    vector<string> all_data;
    for (int i = 0; i < pattern_list.size(); i++) {
        string pattern = pattern_list[i];

        if(i==0)
            all_data = get_result(pattern, bwt_file, index_file);
        else
            all_data = _find_pattern_in_list(pattern, all_data);
    }
    for(int i=0; i<all_data.size();i++)
        printf("%s", all_data[i]);
    return 0;
}

vector<string> get_result(string pattern, string input_file, string _index_file) {

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



    for(int i = first; i <= last; ++i){
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
    string line;
    while(i < line_num){
        getline(index_file, line);
        i++;
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
    bwt_file.seekg(line_num - extra_num,ios::beg);
    for(int i=0; i<extra_num; ++i){
        bwt_file>>noskipws>>key;
        if(key == ch)
            value++;
    }
    return value;
}


