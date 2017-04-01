#include <iostream>
#include <fstream>
#include <cstring>
#include <map>

const int K_value = 1000;
const int MAX_NUM_OF_INDEX = 1000;

using namespace std;
/*
 * 怎么生成index，需要n^2的空间。至少2n。
 *
 *
 */

map<char, int> build_index(string input_file,string index_file);
map<char, int> occ_counter;
int size_recorder[MAX_NUM_OF_INDEX];

int main(int argc, char* argv[]) {

    /* initialized */
    string input_file = argv[1], index_file = argv[2];
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
    int i=0, j=0;
    while(my_file.get(a)){
        count_list[a]++;
        occ_counter[a] += 1;
        i++;

        if(i>K_value){

            int size = sizeof('a')*3;
            int total_size = size_recorder[j-1];
            for(map<char,int>::iterator it = occ_counter.begin(); it != occ_counter.end(); ++it){
                index_file<<it->first<<" "<<it->second<<endl;
                total_size += size;
            }
            size_recorder[j] = total_size;
            occ_counter.clear();
            j++;
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

