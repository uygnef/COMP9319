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


#define LAST_CHAR 127
const int BLOCK_BYTES = 10;
const int MAX_NUM_OF_INDEX = 1000;
using namespace std;

map<char, int> occ_counter;
map<char, int> get_C_from_index_file(string index_file);
map<char, int> count_map;
int occ(char ch, int num, fstream& bwt_file, fstream& index_file);
int get_result(string pattern, string input_file, string _index_file);
char get_char(int i, fstream& bwtfile);
string backward_search(int i, string pattern, fstream& bwt_file, fstream& index_file);

int main(int argc, char* argv[]) {

    printf("start\n");

    /* initialized */
    string input_file = argv[1], index_file = argv[2];
    if (argc <= 3){
        printf("less than 3 parameter:\n Usage: input index pattern ...\n");
        return -1;

    }
    vector<string> pattern(argc-3);
    for(int i=3; i<argc; i++){
        pattern[i-3] = argv[i];
        cout<<pattern[0]<<endl;
    }

    /*create index file*/

    count_map = get_C_from_index_file(index_file);

    int err = get_result(pattern[0], input_file, index_file);
    if(err)
        return -1;
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

int get_result(string pattern, string input_file, string _index_file) {

    int curt_pos = pattern.size() - 1;
    char c = pattern[curt_pos];
    int first = count_map[c];

    if(first == 0){
        printf("NO SUCH FILE");
        return -1;
    }

    int last = 0;
    char d = c+1;
    auto next_char = count_map.find(d);
    while(true){ //get next char in map
        if(d == LAST_CHAR){
            last = count_map[(char)LAST_CHAR] - 1; //this is the last element.
            break;
        }
        if(next_char != count_map.end()){
            last = count_map[d];
            break;
        }

        d++;
        next_char = count_map.find(d);
    }

//load index file
    fstream index_file;
    index_file.open(_index_file);

    fstream bwt_file;
    bwt_file.open(input_file);
    vector<string> result; //list to store full string(from [] to end)
    printf("c is %c\nfirst is %d, last is %d, curt_pos is %d\n",c, first, last, curt_pos);
    printf("_____________\n");
    while (curt_pos >= 1 && first <= last) {
        char a,b;
        c = pattern[curt_pos-1];
        printf("c is %c. ", c);
        first = count_map[c] + occ(c, first-1, bwt_file, index_file);
        printf("first: %d = %d + %d\n", first, count_map[c], first - count_map[c]);
        last = count_map[c] + occ(c, last, bwt_file, index_file) - 1;
        printf("last: %d = ctm(%d) + occ(%d)\n", last,count_map[c], last -count_map[c] + 1);

        if(last<first){
            printf("first is %d, last is %d", first, last);
            return -1;
        }
        bwt_file.seekg(first);
        bwt_file>>noskipws>>a;
        bwt_file.seekg(last);
        bwt_file>>noskipws>>b;
        printf("first %d char is %c, last %d is %c\n", first, a, last, b);
        printf("curt_pos is %d\n", curt_pos);
        curt_pos--;
    }


    for(int i = first; i <= last; i++) {
        string match_prefix = backward_search(i, pattern, bwt_file, index_file);
    }

    return 0;
}

string backward_search(int i, string pattern, fstream& bwt_file, fstream& index_file){
    string full_word;
    full_word.assign(pattern, 0 , 1);
    cout<<"***\n"<<full_word<<"\n";
//backward search find first part
    char pre_ch = get_char(i, bwt_file);
    int pre_ch_num = i;
    while (pre_ch != '[') {
        full_word = pre_ch + full_word;
        pre_ch_num = count_map[pre_ch] + occ(pre_ch, pre_ch_num , bwt_file, index_file) - 1;
        pre_ch = get_char(pre_ch_num, bwt_file);
        printf("*******pre_ch_num: %d\tpre_ch %d", pre_ch_num, pre_ch);
    }
    full_word = pre_ch + full_word;
    cout << endl<<endl<<"________________"<<endl <<"full word is: " <<full_word<<endl;
    return full_word;
}


char get_char(int i, fstream& bwtfile){
    bwtfile.clear();
    bwtfile.seekg(i-1, ios::beg);
    char val;
    bwtfile >> noskipws>>val;
    return val;
}

int occ(char ch, int num, fstream& bwt_file, fstream& index_file){
    printf("CALL occ(ch=%c, num=%d):", ch, num);
    int line_num = num / BLOCK_BYTES;
    cout<<"num is "<<line_num<<endl;
    int i = 1;
    //loop to the target line
    string temp;
    index_file.seekg(0);
    while(i < line_num){

        getline(index_file, temp);
    //    cout<<"i: "<<i<<" line_num:"<<line_num<<endl;
        if(index_file.eof()){
            return -1;
        };
        i++;
    }

    int key, value=0;
    map<char, int> last_index;

//get nearest index bucket
    int temp_key = 0;
    while(line_num != 0){
        index_file>>key>>value;
//        cout<<(char)key<<" index value is "<<value<<endl;
        if(temp_key >= key){
//            cout<<"No this word in previous block\n";
            value = 0;
            break;
        }
        temp_key = key;
        if(key == ch){
 //           cout<<"find "<<(char)key<<endl;
            break;
        }
    }

    int extra_num = num % BLOCK_BYTES;
//    printf("seekg is %d\n", line_num*BLOCK_BYTES);
    bwt_file.clear();
    bwt_file.seekg(line_num*BLOCK_BYTES, ios::beg);
    char rd_ch;
    for(int i=0; i<extra_num; ++i){
        if(rd_ch == EOF)
            break;
        bwt_file>>noskipws>>rd_ch;
        if(rd_ch == ch)
            value++;
    }
    cout<<" value:"<<value<<endl;
    return value;
}