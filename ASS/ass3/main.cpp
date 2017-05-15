#include <iostream>
#include <array>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <dirent.h>
#include "smassung-porter2_stemmer/porter2_stemmer.h"
#include "smassung-porter2_stemmer/porter2_stemmer.cpp"

using namespace std;

#define CREATE_INDEX_DEBUG
#define INPUT_DEBUG

#define ALL_READ_BLOCK_SIZE 15000000
void add_one(map<string, map<int, int>>& index, string word, int file_no);
void update_index(map<string, map<int, int>>& index, string files, int file_no);
int create_index( map<string, map<int, int>>& index, vector<string> file_list);
vector<string> get_all_files(string path);
void write_index_to_file(string file_name, map<string, map<int, int>>& index);
int load_block(vector<string>& store_list, fstream& file, const int block_size);
string load(short& index_pos, vector<string>& index_line, fstream& index_file, int block_size);
void compare_and_write(string queue[], vector<short>& remain, fstream& index_name);
string get_key(string line);
string get_value(string line);
void merge_string(string& a, string& b);
void write_down(string& line, fstream& index_file);
void merge_all_block(vector<string> index_line[], string index_name, fstream index_file[], const int each_block_size);
void merge_index(string index_name);

int memory_counter = 0;
short index_no = 0;//TODO might be wrong.


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
#ifndef INPUT_DEBUG
    bool concept = true;
    if(argv[2] != "-c"){
        concept = false;
    }
    string path = argv[1];

#endif
    cout<<"start"<<endl;
    vector<string> files;
    map<string, map<int, int>> index;


#ifndef CREATE_INDEX_DEBUG
    files.push_back("../asset/simple/file1.txt");
    files.push_back("../asset/simple/file2.txt");
    files.push_back("../asset/simple/file3.txt");

    create_index(index, files);

    for(auto it = index.begin(); it != index.end(); ++it)
    {
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            cout << it->first << " " << it2->first << " " << it2->second << "\n";
        }
    }
#else
    string path = "../asset/books200m";
    files = get_all_files(path);
//create_index(index, files);

    index_no = 13;
    printf("start merge index.\n");
    string merge_index_name = "my.index";
    merge_index(merge_index_name);
//    pattern = get_pattern(argv[]);
//    search(pattern);
#endif
    return 0;
}

int create_index(map<string, map<int, int>>& index, vector<string> file_list){

    for(int i=0; i < file_list.size(); ++i){
#ifdef CREATE_INDEX_DEBUG
        cout<<file_list[i]<<endl;
#endif
        update_index(index, file_list[i], i);

    }
    printf("Write LAST sub INDEX.\n");
    if(not index.empty()){ //deal with the last index in memory.
        write_index_to_file(to_string(index_no), index);
        index_no++;
    }


    printf("start merge index.\n");
    string merge_index_name = "my.index";
    merge_index(merge_index_name);

}

void update_index(map<string, map<int, int>>& index, string files, int file_no){
    fstream file;
    string word;

    file.open(files, fstream::in);
    if(file.fail()){
        cout<<"READ FAIL: "<<files<<endl;
        return;
    }

    string divider = "";
    for(char i=0; i<126; ++i){
        if((i<='Z' and i>='A') or (i>='a' and i<='z'))
            continue;
        divider += i;
    }

    file.imbue(std::locale(std::locale(), new field_reader(divider)));

    while(file>>word){
#ifndef CREATE_INDEX_DEBUG
        cout<<word<<" ";
#endif
        Porter2Stemmer::trim(word);
        Porter2Stemmer::stem(word);
#ifndef CREATE_INDEX_DEBUG
        cout<<word<<endl;
#endif
        add_one(index, word, file_no);
/*
 * when have used all memory, write index to the disk.
 */
        if (memory_counter>15000000){ //make sure will not run out of memory
            write_index_to_file(to_string(index_no), index);
            index_no++;
            return;
        }
    }

}

void add_one(map<string, map<int, int>>& index, string word, int file_no){
    map<string, map<int, int>>::iterator it;
    map<int, int>::iterator it_2;

    it = index.find(word);
    if (it == index.end()){
        map<int, int> occur;
        occur.insert(pair<int, int>(file_no, 1));
        index.insert(pair<string, map<int, int>>(word, occur));
        memory_counter += (word.size() + 4 + 4);
    }else{

        it_2 = it->second.find(file_no);
        if(it_2 == it->second.end()){
            it->second.insert(pair<int, int>(file_no, 1));
            memory_counter += 4 + 4;
        }else{
            it_2->second += 1;
        }
    }
    memory_counter += 5;
}

/* Returns a list of files in a directory (except the ones that begin with a dot) */

vector<string> get_all_files(string path) {

    DIR*    dir;
    dirent* pdir;
    vector<string> files;

    dir = opendir(path.c_str());

    while (pdir = readdir(dir)) {
//        cout<<pdir->d_name<<endl;
        if(pdir->d_name[0] == '.') continue;
#ifndef INPUT_DEBUG
        cout<<"THIS IS "<<pdir->d_name<<endl;
#endif
        files.push_back(path + '/' + pdir->d_name);
    }
    return files;
}


/*
 * write index to file:
 * when memory is full, put index map into the file
 */
void write_index_to_file(string file_name, map<string, map<int, int>>& index){
    ofstream file;
    file.open(file_name);
    if(file.fail()){
        printf("OPEN INDEX FILE ERROR.\n");
        return;
    }
    printf("WRITE A index %s.\n", file_name);
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
        rename("0", index_name.c_str());
        return;
    }
    const int each_block_size = ALL_READ_BLOCK_SIZE/index_no;

    fstream index_file[index_no];           //store open file pointer
    vector<string> index_line[index_no];    //store the line in each index file;

    for(int i=0; i<index_no; ++i){
        printf("index %d: initialized.\n");
        string temp_index = to_string(i);
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
        printf("SUB INDEX LIST NOT EMPTY.\n");
        return -1;
    }
    /*file is closed, means has merge this file.
     *
     */
    printf("load block.\n");

    if(!file.is_open()){
        printf("--------has closed!\n");
        return 0;
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
    printf("load finished.\n");
    return 0;
}

/*
 * use the string in memory, merge into a list, after the index list is full,
 * put it into the disk.
 */
void merge_all_block(vector<string> index_line[], string index_name, fstream index_file[], const int each_block_size){
    printf("merge all block.\n");

    short index_pos[index_no] = {0};
    string queue[index_no];

    for(short i=0; i<index_no; ++i){ // load data to queue. initialized
            queue[i] = load(index_pos[i], index_line[i], index_file[i], each_block_size);
    }
    vector<short> remain_file;//if remain file is empty, merge is done.
    for(short i=0; i < index_no; i++){
        remain_file.push_back(i);
    }

    fstream file;
    file.open(index_name, ios::in | ios::out | ios::app);
    if(file.fail()){
        printf("OPEN INDEX FILE ERROR.");
        return;
    }
    while(!remain_file.empty()){
        compare_and_write(queue, remain_file, file);
        for(auto i:remain_file){
             if(queue[i].empty())
                 queue[i] = load(index_pos[i], index_line[i], index_file[i], each_block_size);
        }
    }
}


string load(short& index_pos, vector<string>& index_line, fstream& index_file, int block_size){
    if (index_pos == index_line.size()){
        index_line.clear();
        index_pos = 0;
        if(load_block(index_line, index_file, block_size)){
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
    vector<short>::iterator temp_i;
    for(vector<short>::iterator i = remain.begin(); i != remain.end(); ++i){
        if(queue[*i].compare("EOF")){
            remain.erase(i);
            continue;
        }
        if(min_str.empty()){
            min_str = get_key(queue[*i]);
            temp_i = i;
            continue;
        }
        temp = get_key(queue[*i]);
        if(temp.compare(min_str) < 0){
            min_str = temp;
            temp_i = i;
        }

        if(temp.compare(min_str) == 0)               //if two is equal merge them to a new set.
            merge_string(queue[*temp_i], queue[*i]);

    }

    write_down(queue[*temp_i], index_name);
}


void merge_string(string& a, string& b){
    a = get_key(a) + get_value(a) + get_value(b) + "\n";
    b.clear();
}

string get_key(string line){
    string a="";
    for(auto i: line){
        if(i == ' ') break;
        a += i;
    }
    return a;
}

string get_value(string line){
    string a="";
    bool value = false;
    for(auto i: line){
        if(i != ' ' and !value) {
            value = true;
            continue;
        }
        if(i == '\n')
            break;
        a += i;
    }
}

void write_down(string& line, fstream& index_file){
    index_file<<line;
    line.clear();
}