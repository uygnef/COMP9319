#include <iostream>
#include <array>
#include <string>
#include <map>
#include <fstream>

using namespace std;

void add_one(map<string, map<string, int>> index, string word, string file);
void update_index(map<string, map<string, int>> index, string files);
int create_index(string file_list[], int num);

int main(int argc, char* argv[]) {
    map<string, map<string, int>> index;
	string file[3] = {"../asset/simple/file1.txt", "../asset/simple/file2.txt", "../asset/simple/file3.txt"};
	int num = 3;
	create_index(file, 3);
	for(auto it = index.begin(); it != index.end(); ++it)
	{
		cout << it->first << " " << it->second.first << " " << it->second.second << "\n";
	}

}

int create_index(string file_list[], int num){

    map<string, map<string, int>> index;
    for(int i=0; i < num; ++i){
        string file = file_list[i];
        update_index(index, file);
    }
}

void update_index(map<string, map<string, int>> index, string files){
    fstream file;
    string word;

    file.open(files, fstream::in);

    while(file>>boolalpha>>word){
        add_one(index, word, files);
    }
}

void add_one(map<string, map<string, int>> index, string word, string file){
    map<string, map<string, int>>::iterator it;
    map<string, int>::iterator it_2;

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
