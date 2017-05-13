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

void add_one(map<string, map<string, int>>& index, string word, string file);
void update_index(map<string, map<string, int>>& index, string files);
int create_index( map<string, map<string, int>>& index, vector<string> file_list);
void get_all_files(vector<string> &files, string path);
void clean_path(string &path);


int main(int argc, char* argv[]) {
#ifndef CREATE_INDEX_DEBUG
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
    get_all_files(files, path);
    create_index(index, files);
    pattern = get_pattern(argv[]);
    search(pattern);
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

    while(file>>boolalpha>>word){
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
        cout<<pdir->d_name;
        files.push_back(pdir->d_name);
    }
}