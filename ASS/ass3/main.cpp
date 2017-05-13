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
void add_one(map<string, map<string, int>>& index, string word, string file);
void update_index(map<string, map<string, int>>& index, string files);
int create_index( map<string, map<string, int>>& index, vector<string> file_list);
void get_all_files(vector<string> &files, string path);
void clean_path(string &path);




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
    map<string, map<string, int>> index;


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
    string path = "../asset/simple";
    get_all_files(files, path);
    create_index(index, files);
//    pattern = get_pattern(argv[]);
//    search(pattern);
#endif
    return 0;
}

int create_index(map<string, map<string, int>>& index, vector<string> file_list){

    for(int i=0; i < file_list.size(); ++i){
        cout<<file_list[i]<<endl;
        string file = file_list[i];
        update_index(index, file);
    }

}

void update_index(map<string, map<string, int>>& index, string files){
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
        cout<<word<<" ";
        Porter2Stemmer::trim(word);
        Porter2Stemmer::stem(word);
        cout<<word<<endl;
        add_one(index, word, files);
    }
}

void add_one(map<string, map<string, int>>& index, string word, string file){
    map<string, map<string, int>>::iterator it;
    map<string, int>::iterator it_2;

    clean_path(file);
    it = index.find(word);
    if (it == index.end()){

        map<string, int> occur;
        occur.insert(pair<string, int>(file, 1));
        index.insert(pair<string, map<string, int>>(word, occur));
    }else{

        it_2 = it->second.find(file);
        if(it_2 == it->second.end()){
            it->second.insert(pair<string, int>(file, 1));
        }else{
            it_2->second += 1;
        }
    }
}

/*only keep the file name from path*/

void clean_path(string &path){
    size_t pos = path.find_last_of ('/');
    if (pos != string::npos)
        path = path.substr(pos+1);
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