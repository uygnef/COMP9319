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

//#define CREATE_INDEX_DEBUG
#define INPUT_DEBUG

#define ALL_READ_BLOCK_SIZE 15000000
void add_one(map<string, map<int, int>>& index, string word, int file_no);
void update_index(map<string, map<int, int>>& index, string files, int file_no);
int create_index( map<string, map<int, int>>& index, vector<string> file_list);
void get_all_files(vector<string> &files, string path);
void write_index_to_file(string file_name, map<string, map<int, int>>& index);

int memory_counter = 0;
int index_no = 0;

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


#ifdef CREATE_INDEX_DEBUG
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
    get_all_files(files, path);
    create_index(index, files);
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
    if(not index.empty()){
        write_index_to_file(to_string(index_no), index);
        index_no++;
    }

    merge_index(index_no);

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
#ifdef CREATE_INDEX_DEBUG
        cout<<word<<" ";
#endif
        Porter2Stemmer::trim(word);
        Porter2Stemmer::stem(word);
#ifdef CREATE_INDEX_DEBUG
        cout<<word<<endl;
#endif
        add_one(index, word, file_no);
/*
 * when have used all memory, write index to the disk.
 */
        if (memory_counter>15000000){ //make sure will not run out of memory
            write_index_to_file(to_string(index_no), index);
            index_no++;
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

void get_all_files(vector<string> &files, string path) {

    DIR*    dir;
    dirent* pdir;

    dir = opendir(path.c_str());

    while (pdir = readdir(dir)) {
//        cout<<pdir->d_name<<endl;
        if(pdir->d_name[0] == '.') continue;
#ifndef INPUT_DEBUG
        cout<<"THIS IS "<<pdir->d_name<<endl;
#endif
        files.push_back(path + '/' + pdir->d_name);
    }
}


/*
 * write index to file:
 * when memory is full, put index map into the file
 */
void write_index_to_file(string file_name, map<string, map<int, int>>& index){
    ofstream file;
    file.open(file_name);
    for(auto it = index.begin(); it != index.end(); ++it)
    {
        file << it->first ;
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            file << " " << it2->first << " " << it2->second;
        }
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
    const int each_block_size = ALL_READ_BLOCK_SIZE/index_no;

    fstream index_file[index_no];           //store open file pointer
    array<string, each_block_size> index_line[index_no];    //store the line in each index file;

    for(int i=0; i<index_no; ++i){
        string temp_index = to_string(i);
        index_file[i].open(temp_index.c_str(), fstream::in);

    }

}