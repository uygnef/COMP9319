#include <iostream>
#include <array>
#include <cstring>
#include <map>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <set>

extern "C"{
#include "stmr.h"
}


using namespace std;

//#define CREATE_INDEX_DEBUG
//#define INPUT_DEBUG
//#define CONCEPT_DEBUG
#define ALL_READ_BLOCK_SIZE 10000000
/*
 * concept threshold is the vector angle of two word vector.
 *
 * each word has a vector i, i is (file, frequency) pair.
 * when do concept searching, I go through the whole dictionary, calculte the vector angle between every
 * word and a single search pattern, if it larger than CONCEPT_THRESHOLD, I let it be a concept search pattern.
 */
#define CONCEPT_THRESHOLD 0.05

//if you want to check which concept word I have choose, please define CONCEPT_DEBUG.
//#define CONCEPT_DEBUG


void add_one(map<string, vector<pair<int, int>>>& index, string word, int file_no);
int create_index(vector<string> file_list, string merge_index_name);
void update_index(map<string, vector<pair<int, int>>>& index, string files, short file_no);
vector<string> get_all_files(string path);
void write_index_to_file(string file_name, map<string, vector<pair<int, int>>>& index);
int load_block(vector<string>& store_list, fstream& file, const int block_size);
string load(int& index_pos, vector<string>& index_line, fstream& index_file, int block_size);
void compare_and_write(string queue[], vector<short>& remain, fstream& index_name);
string get_key(string line);
string get_value(string line);
void merge_string(string& a, string& b);
void write_down(string& line, fstream& index_file);
void merge_all_block(vector<string> index_line[], string index_name, fstream index_file[], const int each_block_size);
void merge_index(string index_name);
string search_pattern(long long start, long long end, fstream& file, string pattern);
vector<pair<int,int>>  split(string str);
bool contain(vector<pair<int,int>> result, short& pos, int compare);
map<int, int> check_file(vector<pair<int,int>> result[], short len,  bool concept = false, float rate=0, short pos=0);
multimap<int, int> search_word(string pattern[], fstream& file, short len);
string clean_path(string path);
void trans_stem(string& word);
float abs_length(string line);
float concept_compare(string line, vector<pair<int,int>>& pattern, int line_total);
multimap<int, int> concept_search(string pattern[], fstream& index_file, short len, float rate);
void update_result(map<int, int>& concept_result, map<int, int>& final_result);

int memory_counter = 0; //counter: after run out of memory, write sub index into disk.
int index_no = 0;//counter: record  the number of index.
string folder_prefix;

struct field_reader: std::ctype<char> {
    field_reader(std::string const &s): std::ctype<char>(get_table(s)) {}
    static std::ctype_base::mask const* get_table(std::string const &s) {
        static std::vector<std::ctype_base::mask>
                rc(table_size, std::ctype_base::mask());
        for (auto ch : s)
            rc[ch] = std::ctype_base::space;
        return &rc[0];
    }
};


int main(int argc, char* argv[]) {
#ifndef NAIVE_CONCEPT_SEARCH
    bool concept = true;
    int word_len;
    float rate;
    if(strcmp(argv[3], "-c") != 0){
        ////cout<<"NOT -c"<<endl;
        concept = false;
        word_len = argc - 3;
    }else{
        word_len = argc -5;
        rate = stof(argv[4]);
    }
    ////cout<<word_len<<endl;
    string path = argv[1];
    string word[word_len];
//    for(int i=0;i<argc;i++){
//        cout<<i<<" "<<argv[i]<<endl;
//    }
    folder_prefix = argv[2];
    folder_prefix += "/";
//    cout<<"FOLDER IS:"<<folder_prefix<<endl;
    for(int i= 0; i<word_len; i++){
        if(concept)
            word[i] = argv[5 + i];
        else
            word[i] = argv[3+i];
        trans_stem(word[i]);
    }

    map<string, vector<pair<int, int>>> index;


#ifdef CREATE_INDEX_DEBUG
    files.push_back("../asset/simple/file1.txt");
    files.push_back("../asset/simple/file2.txt");
    files.push_back("../asset/simple/file3.txt");

    create_index(index, files);

    for(auto it = index.begin(); it != index.end(); ++it)
    {
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            ////cout << it->first << " " << it2->first << " " << it2->second << "\n";
        }
    }
#else

    ////cout<<"PATH IS: "<<path<<endl;
    vector<string> files = get_all_files(path);
    for(auto i = 0; i < files.size();i++){
        if(clean_path(files[i]).compare("10226_10226.txt") ==0)
            cout<<i<<endl;
    }
#ifndef MERGE_TEXT

    if(access(folder_prefix.c_str(), F_OK) == -1){

   //     cout<<"No FOLDER IS:"<<folder_prefix<<endl;
        mkdir(folder_prefix.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    }

    string merge_index_name = "fengyu.index";
    ifstream infile((folder_prefix + merge_index_name).c_str());
    if(!infile.good()) {
        create_index(files, merge_index_name);
    }
#else
    index_no = 13;  //test for merge index
    //printf("start merge index.\n");
    string merge_index_name = "my.index";
    merge_index(merge_index_name);
//    pattern = get_pattern(argv[]);
//    search(pattern);
#endif
#endif
    fstream file;
    file.open((folder_prefix + merge_index_name).c_str(), ios::in|ios::out);
    if(!file.good()){
        //cout<<endl;
        return -1;
    }

    multimap<int, int> result;
    if(concept)
        result = concept_search(word, file, word_len, rate);
    else
        result = search_word(word, file, (short)word_len);


    set<pair<int, int>> res;
    for (multimap<int, int>::iterator i = result.end(); i != result.begin();) {
        i--;
        res.insert(pair<int, int>(i->first, -i->second));
    }

    for (set<pair<int, int>>::iterator i = res.end(); i != res.begin();) {
        i--;
        cout << clean_path(files[-i->second])<<endl;
    }
    if(result.empty()){
        cout<<endl;
    }
#endif

    return 0;
}

bool stringCompare( const string &left, const string &right ){
    for( string::const_iterator lit = left.begin(), rit = right.begin(); lit != left.end() && rit != right.end(); ++lit, ++rit )
        if( tolower( *lit ) < tolower( *rit ) )
            return true;
        else if( tolower( *lit ) > tolower( *rit ) )
            return false;
    if( left.size() < right.size() )
        return true;
    return false;
}
string clean_path(string path){
    string temp;
    for(auto i:path){
        if(i == '/')
            temp = "";
        else
            temp += i;
    }
    return temp;
}

int create_index(vector<string> file_list, string merge_index_name){
    map<string, vector<pair<int, int>>> index;
    for(short i=0; i < file_list.size(); ++i){
#ifdef CREATE_INDEX_DEBUG
        //cout<<file_list[i]<<endl;
#endif
        update_index(index, file_list[i], i);

    }
    //printf("Write LAST sub INDEX.\n");
    if(not index.empty()){ //deal with the last index in memory.
        write_index_to_file(to_string(index_no), index);
        index_no++;
        index.clear();
    }

    merge_index(merge_index_name);

}

void update_index(map<string, vector<pair<int, int>>>& index, string files, short file_no){
    fstream file;
    string word;

    file.open(files, fstream::in);
    if(file.fail()){
        //cout<<"READ FAIL: "<<files<<endl;
        return;
    }

    string divider = "";
    for(char i=0; i<=126; ++i){
        if((i<='Z' and i>='A') or (i>='a' and i<='z'))
            continue;
        divider += i;
    }
    divider += '~';
    file.imbue(std::locale(std::locale(), new field_reader(divider)));
    while(file>>word){
#ifdef CREATE_INDEX_DEBUG
        //cout<<word<<" ";
#endif
        if(word.length() < 3)
            continue;

        transform(word.begin(), word.end(), word.begin(), ::tolower);
        string temp_word = word;
        char *cstr = &word[0u];
        cstr[stem(cstr, 0, word.size() -1) + 1] = 0;
        word = string(cstr);

#ifdef CREATE_INDEX_DEBUG
       // if(word.compare(temp_word) != 0)
            //cout<<temp_word<<" "<<word<<endl;
        //cout<<word<<endl;
#endif
        add_one(index, word, file_no);
/*
 * when have used all memory, write index to the disk.
 */
        if (memory_counter>8000000){ //make sure will not run out of memory
            write_index_to_file(to_string(index_no), index);
            //cout<<index_no<<" BREAK AT: "<<file_no<<endl;
            index_no++;
        }
    }

}
/*
 * stem word
 */

void trans_stem(string& word){
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    string temp_word = word;
    char *cstr = &word[0u];
    cstr[stem(cstr, 0, word.size() -1) + 1] = 0;
    word = string(cstr);
}



void add_one(map<string, vector<pair<int, int>>>& index, string word, int file_no){
    map<string, vector<pair<int, int>>>::iterator it;

    it = index.find(word);

    if (it == index.end()){

        vector<pair<int, int>> list;
        list.push_back(pair<int, int>(file_no, 1));
        index.insert(pair<string, vector<pair<int, int>>>(word, list));
        memory_counter += (word.size() + 20);
    }else{
        if(file_no == it->second.back().first)
            it->second.back().second += 1;
        else{
            it->second.push_back(pair<int, int>(file_no, 1));
            memory_counter += 20;
        }
    }
}

/* Returns a list of files in a directory (except the ones that begin with a dot) */

vector<string> get_all_files(string path) {

    DIR*    dir;
    dirent* pdir;
    vector<string> files;

    dir = opendir(path.c_str());

    while (pdir = readdir(dir)) {
//        //cout<<pdir->d_name<<endl;
        if(pdir->d_name[0] == '.') continue;
#ifdef INPUT_DEBUG
        //cout<<"THIS IS "<<pdir->d_name<<endl;
#endif
        files.push_back(path + '/' + pdir->d_name);
    }
    return files;
}


/*
 * write index to file:
 * when memory is full, put index map into the file
 */
void write_index_to_file(string file_name, map<string, vector<pair<int, int>>>& index){
    ofstream file;
    file.open(folder_prefix + file_name);
    if(file.fail()){
        //printf("OPEN INDEX FILE ERROR.\n");
        return;
    }
    //printf("WRITE A index.\n");
    for(auto it = index.begin(); it != index.end(); ++it)
    {
        file << it->first ;
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            file << " " << it2->first << " " << it2->second;
        file<<endl;
    }

    index.clear();
    memory_counter = 0;
}

/*
 * merge index:
 * connect two index together. get an new index.
 */

/*
 *  open all index file;
    open index_name file;
    load fix blocked from all index file to memory;

    compare all data line by line;
    if memory index be max size, write it to memory continue loop;

    all file done, remove all file
 */

void merge_index(string index_name){

    if (index_no == 1){ //if only have one index, just rename it and return.
        string trans_name = folder_prefix + "0";
        rename(trans_name.c_str(), (folder_prefix + index_name).c_str());
        return;
    }
    const int each_block_size = ALL_READ_BLOCK_SIZE/index_no; //set buffer fir each sub index file;

    fstream index_file[index_no];           //store open file pointer
    vector<string> index_line[index_no];    //store the line in each index file;

    for(int i=0; i<index_no; ++i){
        string temp_index = folder_prefix + to_string(i);
        index_file[i].open(temp_index.c_str(), fstream::in);
        load_block(index_line[i], index_file[i], each_block_size);
    }

    merge_all_block(index_line, index_name, index_file, each_block_size);
}

/*
 * maintain a list which get index from a sub_index file,
 * when the current index is used, move to next index entity.
 * after all index has used, clear the list and load next block
 * data from sub_index file.
 */
int load_block(vector<string>& store_list, fstream& file, const int block_size){
    if(!store_list.empty()){
        //printf("SUB INDEX LIST NOT EMPTY.\n");
        return -1;
    }
    /*file is closed, this file has been merged .
     *
     */
    //printf("load block.\n");

    if(!file.is_open()){
        //printf("--------has closed!\n");
        return 1;
    }
    int size = 0;
    string temp;
    while(getline(file, temp)){
        store_list.push_back(temp);

        size += (temp.size() + 2);
        if(size > block_size){
            return 1;
        }
    }
    file.close();
    //printf("load finished.\n");
    return 0;
}

/*
 * use the string in memory, merge into a list, after the index list is full,
 * put it into the disk.
 */
void merge_all_block(vector<string> index_line[], string index_name, fstream index_file[], const int each_block_size){
    //printf("merge all block.\n");

    int index_pos[index_no];
    for(short i=0; i<index_no; ++i){
        index_pos[i] = 0;
    }
    string queue[index_no];

    for(short i=0; i<index_no; ++i){ // load data to queue. initialized
        queue[i] = load(index_pos[i], index_line[i], index_file[i], each_block_size);
    }
    vector<short> remain_file;//if remain file is empty, merge is done.
    for(short i=0; i < index_no; ++i){
        remain_file.push_back(i);
    }

    fstream file;
    file.open(folder_prefix + index_name, ios::out);
    if(file.fail()){
        //printf("OPEN INDEX FILE ERROR.");
        return;
    }
    //cout<<"index_name";
    while(!remain_file.empty()){
        compare_and_write(queue, remain_file, file);
        for(auto i:remain_file){
            if(queue[i].empty())
                queue[i] = load(index_pos[i], index_line[i], index_file[i], each_block_size);
        }
    }
}


string load(int& index_pos, vector<string>& index_line, fstream& index_file, int block_size){
    if (index_pos == index_line.size()){
        index_line.clear();
        index_pos = 0;
        if(load_block(index_line, index_file, block_size) != 1){
            index_pos = 0;
            string data = index_line[0];
            index_pos++;
            return data;
        }
        return "EOF";
    }

    string data = index_line[index_pos];
    index_pos++;
    return data;
}

/*
 * compare the all element in queue, put the smallest into disk.
 * remove this element.
 */
void compare_and_write(string queue[], vector<short>& remain, fstream& index_name){
    string min_str;
    string temp;
    vector<short>::iterator min_i;
    for(vector<short>::iterator i = remain.begin(); i != remain.end(); ++i){
//        //cout<<"QUEUE IS: "<<queue[*i]<<endl;
        if(queue[*i].compare("EOF") == 0){
            //cout<<"HAS CLOSED "<<*i<<endl;
            int temp_name = *i;
           // remove((folder_prefix + to_string(temp_name)).c_str());
            remain.erase(i);
            return;
        }
        if(min_str.empty()){
            min_str = get_key(queue[*i]);
            min_i = i;
            continue;
        }
        temp = get_key(queue[*i]);
        if(temp.compare(min_str) < 0){
            min_str = temp;
            min_i = i;
            continue;
        }

        if(temp.compare(min_str) == 0){               //if two are equal merge them to a new set.
            merge_string(queue[*min_i], queue[*i]);
        }
    }
    write_down(queue[*min_i], index_name);
}


void merge_string(string& a, string& b){
    if(get_value(a).compare("pear") == 0){
        cout<<1<<a<<" "<<b<<endl;
    }
    a = get_key(a) + get_value(a) + get_value(b);
    if(get_value(a).compare("pear") == 0){
        cout<<2<<a<<" "<<b<<endl;
    }
    b.clear();
}

string get_key(string line){
    string a;
    for(auto i: line){
        if(i == ' ') break;
        a += i;
    }
    return a;
}

string get_value(string line){
    short count = 0;
    for(auto i: line){
        count += 1;
        if(i == ' '){
            count--;
            break;
        }
    }
#ifdef MERGE_DEBUG_GET_VALUE
    //cout<<"LINE IS: "<<line<<endl;
    //cout<<"COUNT: "<<count<<endl;
    //cout<<"SUB STRING: "<<line.substr(count, line.size() - count + 1)<<endl;
#endif
    return line.substr(count, line.size() - count + 1);
}

void write_down(string& line, fstream& index_file){
    index_file<<line<<endl;
    line.clear();
}

/*
 * load data from index.
 * search required data.
 * binary search.
 *
 */
//void search_pattern(){
//    load_all_pattern_index;
//    select_aviable_target;
//    print_out;
//}
//
//void load all pattern{
//    search_word(pattern, file, index);
//}
//
multimap<int, int> search_word(string pattern[], fstream& file, short len) {

    file.seekg(0, ios::end);
    long long end = file.tellg();
    long long start = 0;
    vector<pair<int, int>> result[len];
    multimap<int, int> file_result;

    //cout<<"search word LEN:"<<len<<endl;
    for (short i = 0; i < len; ++i) {
        string a = search_pattern(start, end, file, pattern[i]);
        //cout<<"string line"<< i<< "is: "<<a<<endl;
        if (a.empty())
            return file_result;
        result[i] = split(get_value(a));
    }

    //cout<<"RESULT SIZE: "<<result->size()<<endl;
    map<int, int> temp_result = check_file(result, len);
    for(auto i=temp_result.begin(); i!=temp_result.end(); i++){
        file_result.insert(pair<int, int>(i->second, i->first));
    }
    return file_result;
}

map<int, int> check_file(vector<pair<int,int>> result[], short len, bool concept, float rate, short pos){
    //cout<<"CHECK FILE\n";
    short num[len];
    vector<pair<int, int>> all;
    for(int i= 0; i<len; i++){
        num[i] = 0;
    }
    map<int, int> ret;

    for(short i=0; i<result[0].size(); i++){
        bool in = true;
        int temp_value = result[0][i].second;
        for(short j=1; j<len; j++){
            if(!contain(result[j], num[j], result[0][i].first)){
                //  //cout<<"NOT CONTAIN\n";
                in = false;
                break;
            }
            if(j == pos and concept)
                temp_value += rate*result[j][num[j]].second;
            else
                temp_value += result[j][num[j]].second;
        }
        if(in) {
            //cout<<"IN: "<<result[0][i].first<<endl;
            ret.insert(pair<int, int>(result[0][i].first, temp_value));
        }
    }
    return ret;
}

bool contain(vector<pair<int,int>> result, short& pos, int compare){
    //cout<<"P: "<<pos<<" size: "<<result.size()<<endl;
    if(pos >= result.size()){
        //cout<<"OUT OF RANGE, POS is "<<pos<<" size is"<<result.size()<<endl;
        return false;
    }
    for(; pos<result.size();){
        if(result[pos].first == compare){
            return true;
        }
        if(result[pos].first < compare)
            pos++;
        else
            return false;
    }
}


string search_pattern(long long start, long long end, fstream& file, string pattern){
    //cout<<"SEARCH PATTERN\n";
    string word;
    if ((end - start) < 100){
        file.seekg(start, ios::beg);
        while(getline(file, word)){
            if(get_key(word).compare(pattern) == 0)
                return word;
            if(file.tellg() > end)
                return "";
        }
    }

    long long now = (start + end)/2;
    file.seekg(now, ios::beg);
    getline(file, word);
    getline(file, word);
    string key = get_key(word);
    //   //cout<<"PATTERN: "<<pattern<<"  search: "<<key<<endl;
    short compare = key.compare(pattern);
    if(compare == 0){
        //       //cout<<"RETURN: "<<word<<endl;
        return word;
    }
    if(compare > 0){
        word = search_pattern(start, now, file, pattern);
    }else{
        word = search_pattern(now, end, file, pattern);
    }
    return word;
}

/*
 * help function:
 */
vector<pair<int,int>>  split(string str){
    stringstream ss(str); // Insert the string into a stream

    vector<pair<int,int>> tokens; // Create vector to hold our words
    pair<int, int> temp = pair<int, int> (-1,0);
    pair<int, int> now;
    while (ss >> now.first and ss>>now.second){
        if(now.first == temp.first){
            temp.second += now.second;
            continue;
        }
        if(temp.first != -1)
            tokens.push_back(temp);
        temp = now;
    }
    if(temp.first != -1)
        tokens.push_back(temp);
    return tokens;
}


/*
 * Concept search.
 *      calculate cos theta of searched word and all word.
 *      select top10 word ...
 */
multimap<int, int> concept_search(string pattern[], fstream& index_file, short len, float rate){
    string line;

    index_file.seekg(0, ios::end);
    long long end = index_file.tellg();
    long long start = 0;
    vector<pair<int, int>> result[len];
    multimap<int, int> file_result;
    float sum[len];

    //cout<<"search word LEN:"<<len<<endl;
    for (short i = 0; i < len; ++i) {
        string a = search_pattern(start, end, index_file, pattern[i]);
        //cout<<"string line"<< i<< "is: "<<a<<endl;
        if (a.empty()){
            pattern[i] = "!";
            continue;
        }
        sum[i] = abs_length(a);    //pre compute length of pattern vector

        result[i] = split(get_value(a));
    }


// get the similar word(cos<x,y> < 0.5)
    vector<string> concept_match[len];
    float temp_cos = 0;
    while(getline(index_file, line)){
        for(short i=0; i < len; ++i){
            if(pattern[i] == "!") continue;

            if(concept_compare(line, result[i], sum[i]) > CONCEPT_THRESHOLD){ //cos theta
#ifdef CONCEPT_DEBUG
                cout<<i<<" " <<pattern[i]<<" ";
#endif
                concept_match[i].push_back(get_value(line));
            }
        }
    }
    map<int, int> concept_result;
    map<int, int> final_result;
    vector<pair<int, int>> temp_pattern[len];
    for(short j=0; j<len; j++) //initialized concept pattern and searched pattern.
        temp_pattern[j] = result[j];

    for(short i=0; i<len; i++){
        for(short j=0; j<concept_match[i].size(); j++){
            temp_pattern[i] = split(concept_match[i][j]);//replace a search pattern with the most similar pattern.
            concept_result = check_file(temp_pattern, len, true,  rate, j);
            update_result(concept_result, final_result);
        }
        temp_pattern[i] = result[i];
    }
    concept_result = check_file(result, len);
    update_result(concept_result, final_result);
    multimap<int, int> ret_result;
    for(auto i=final_result.begin();i != final_result.end(); i++){
        ret_result.insert(pair<int, int>(i->second, i->first));
    }
    return ret_result;
}


void update_result(map<int, int>& concept_result, map<int, int>& final_result){
    for(auto i=concept_result.begin(); i != concept_result.end();i++){
        final_result[i->first] += i->second;
    }
}

float concept_compare(string line, vector<pair<int,int>>& pattern, int line_total){
    stringstream ss(line);
    int patt_total = abs_length(line);
    pair<int, int> temp = pair<int, int> (-1,0);
    pair<int, int> now;
    string word;
    ss >> word;
    if (word.length() <= 3){ //to get rid of (am is are....). dummy stop word...
        return 0;
    }
    if(word.compare("where") & word.compare("which") & word.compare("whose") == 0 ){
        return 0;
    }
    float result = 0;
    vector<pair<int, int>>::iterator pos = pattern.begin();
    while (ss >> now.first and ss>>now.second and pos != pattern.end()){

        while(pos->first < now.first and pos != pattern.end()){
            pos++;
        }
        if(pos != pattern.end() and pos->first > now.first)
            continue;
        result += (now.second * pos->second);
    }

    result = (result/(patt_total*line_total));
#ifdef CONCEPT_DEBUG
    if(result > CONCEPT_THRESHOLD){
        cout<<word<<" "<< result<<" "<< patt_total<<" "<<line_total<<endl;
    }
#endif
    return result;
}

float abs_length(string line){
    stringstream ss(line); // Insert the string into a stream
    vector<pair<int, int>> a = split(get_value(line));
    float sum;
    for( vector<pair<int, int>>::iterator i=a.begin(); i != a.end(); ++i){
        sum += (i->second * i->second);
    }
    return sum;
}

