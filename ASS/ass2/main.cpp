#include <iostream>
#include <fstream>
#include <cstring>
#include <map>

const int K_value = 1000;

using namespace std;
/*
 * 怎么生成index，需要n^2的空间。至少2n。
 *
 *
 */

map<char, int> build_index(string input_file,string index_file);


int main(int argc, char* argv[]) {

    /* initialized */
    string input_file = argv[1];
    string index_file = argv[2];
    if (argc <= 3)
        return -1;
    string pattern[argc-3];
    for(int i=2; i<argc; i++)
        pattern[i-2] = argv[i];

    map<char, int> count_map = build_index(input_file, index_file);
    for( map<char,int>::iterator it = count_map.begin(); it != count_map.end(); ++it)
        cout<<"key: "<<it->first <<" value: "<<it->second<<endl;

    return 0;
}


map<char, int> build_index(string input_file, string output_file){
    ifstream my_file;
    ofstream index_file;
    my_file.open(input_file);
    index_file.open(output_file);

    map<char, int> count_list;
    char a;
    int i=0;
    while(my_file.get(a)){
        count_list[a]++;
        if(i>K_value){

        }
    }


    /*
     * index is |# a a a b b c d d d f| ------->  a:1, b:4, c:6, d:7, f:10
     */
    int temp=0, pre_end=1;
    for( map<char,int>::iterator it = count_list.begin(); it != count_list.end(); ++it){
        temp = it->second;
        it->second = pre_end;
        pre_end += temp;
    }

    return count_list;
}

string bwt(string input_line){

    char ch = input_line[0];
    int i=0;
    while(ch != ']'){
        i++;
        ch = input_line[i];
    }

    string prefix = input_line.substr(0, i);
    string word = input_line.substr(i);
    static int size = word.length();
    string matrix[size];
    string temp = word;
    for(int j=0;j<size;j++){
        matrix[j] = temp;
        temp = temp.substr(j);
    }
    return 0;

}
void print_result(){}

int load_data(string input_file){

    return 0;
}