#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>


#define LAST_CHAR 127

const int BYTES_OF_BLOCK = 1000;
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


map<string,string> get_result(string pattern, string input_file, string _index_file);
int search(vector<string> pattern_list, string bwt_file, string index_file);
int occ(char ch, int num, fstream& bwt_file, fstream& index_file);
char get_char(int i, fstream& bwtfile);
int find_i_char(int i, char ch, fstream& bwtfile, fstream& index_file);
char the_i_th_char(int& i);
string backward_search(int i, fstream& bwt_file, fstream& index_file, string& index);
string forward_search(int i, fstream& bwt_file, fstream& index_file);
void find_pattern_in_list(string pattern, map<string, string>& all_data);


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
    index_file<<127<<" "<<pre_end; //add the ending character(NO.127) to index, like $ in bwt encode.
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
    map<string, string> all_data;
    for (int i = 0; i < pattern_list.size(); i++) {
        string pattern = pattern_list[i];

        if(i==0)
            all_data = get_result(pattern, bwt_file, index_file);
        else
            find_pattern_in_list(pattern, all_data);
    }
    for( map<string,string>::iterator it = all_data.begin(); it != all_data.end(); ++it) {
        cout<<"["<<it->first<<"]"<<it->second<<endl;
    }
    return 0;
}

void find_pattern_in_list(string pattern, map<string, string>& all_data){
    for( map<string,string>::iterator it = all_data.begin(); it != all_data.end(); ++it) {
        if(it->second.find(pattern) == string::npos){
            all_data.erase(it->first);
        }
    }
}


map<string,string> get_result(string pattern, string input_file, string _index_file) {

    int curt_pos = pattern.size() - 1;
    char c = pattern[curt_pos];
    int first = count_map[c];
    map<string, string> all_match;

    if(first == 0){
        return all_match;
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

    while (curt_pos >= 1 && first <= last) {
        c = pattern[curt_pos-1]; //go to pre char in pattern
        first = count_map[c] + occ(c, first-1, bwt_file, index_file);
        last = count_map[c] + occ(c, last, bwt_file, index_file) - 1;
        curt_pos--;
        if(last<first)
            return all_match;
    }

    for(int i = first; i <= last; ++i){
        string index;
        string full_word = backward_search(i, bwt_file, index_file, index);
        full_word += forward_search(i, bwt_file, index_file);
        all_match[index] = full_word;
    }
     //   result[i] = full_word;
    return all_match;
}
 //   bwt_file.close();


string backward_search(int i, fstream& bwt_file, fstream& index_file, string& index){
    string full_word;
//backward search find first part
    char pre_ch = get_char(i, bwt_file);
    int pre_ch_num = i;
    bool is_index = false;
    while (pre_ch != '[') {
        if(pre_ch == ']'){
            is_index = true;
        }
        if(is_index){
            if(pre_ch != ']') index = pre_ch + index;
        }else{
            full_word = pre_ch + full_word;
        }
        pre_ch_num = count_map[pre_ch] + occ(pre_ch, pre_ch_num , bwt_file, index_file) - 1;
        pre_ch = get_char(pre_ch_num, bwt_file);
    }
    return full_word;
}


string forward_search(int i, fstream& bwt_file, fstream& index_file){
    char  ch = the_i_th_char(i);
    string full_word = "";
    while(ch != '['){
        full_word += ch;
        i = find_i_char(i, ch, bwt_file, index_file);
        ch = the_i_th_char(i);
    }
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

    int line_num = num / BYTES_OF_BLOCK;
    int i = 1;
   //loop to the target line
    string temp;
    index_file.clear();
    while(i < line_num ){
        getline(index_file, temp);
        if(index_file.eof()){
            printf("NUM OUT OF INDEX");
            return -1;
        }
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

    int extra_num = num % BYTES_OF_BLOCK;
    bwt_file.clear();
    bwt_file.seekg(line_num*BYTES_OF_BLOCK,ios::beg);
    char bwt_ch;
    for(int i=0; i<extra_num; ++i){
        if(bwt_ch == EOF)//TODO: in case out of index
            break;
        bwt_file>>noskipws>>bwt_ch;
        if(bwt_ch == ch)
            value++;
    }
    cout<<" value:"<<value<<endl;
    return value;
}

int find_i_char(int i, char ch, fstream& bwtfile, fstream& index_file){
 //   printf("---------find i char : i is %d, ch is %c\n", i, ch);

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
  //          printf("key is %c, value is %d\n", (char)key, value);
            if(key == ch) {
                if (value < i && value>result) {
                    result = value;
 //                   printf("result: %d\n", result);
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
    int ret = line_num*BYTES_OF_BLOCK;
 //   printf("ret is %d, result is %d\n", ret, result);

    bwtfile.clear();
    bwtfile.seekg(ret);
    char chr;
    while(result != i && !bwtfile.eof()){
        if(result > i){
 //           printf("find %d char ERR: OUT OF INDEX %d\n", result, i);
            return -1;
        }
        bwtfile>>noskipws>>chr;
 //       cout<<chr<<endl;
        if(chr == ch){
            result++;
        }
        ret++;
    }
    return ret;
}

char the_i_th_char(int& i){
 //   printf("---------the i the char : i is %d\n", i);
    char ch, temp_ch;
    int temp_value;
    for( map<char,int>::iterator it = count_map.begin(); it != count_map.end(); ++it){
        temp_ch = it->first;
        temp_value = it->second;
//        printf("---------temo_ch %c, temp value %d\n", temp_ch, temp_value);
        if(temp_value > i)
            break;
        ch = temp_ch;
    }
    i = i - count_map[ch] + 1;
//    printf("---------char is %c\n", ch);
    return ch;
}