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

const int BLOCK_BYTES = 10;
const int MAX_NUM_OF_INDEX = 1000;
using namespace std;

int occ(char ch, int num, fstream& bwt_file, fstream& index_file);

int main(int argc, char* argv[]) {

    /* initialized */
string arg_2 = argv[2];


char c = argv[1][0];
int num = std::stoi(arg_2);

    /*create index file*/
    fstream  _file, bwt_file;
    _file.open("index.file",ios::in);
	bwt_file.open("input.bwt");
	
	int a = occ(c, num, bwt_file, _file);
	cout<<endl<<"result is:"<<a<<endl;
}

int occ(char ch, int num, fstream& bwt_file, fstream& index_file){

    int line_num = num / BLOCK_BYTES;
	cout<<"num is "<<line_num<<endl;
    int i = 1;
   //loop to the target line
    string temp;
    index_file.seekg(0);
    while(i < line_num){
        getline(index_file, temp);
        cout<<temp<<endl;
		if(index_file.eof()){
			return -1;
		}
        i++;
    }

    int key, value = 0;
    map<char, int> last_index;

//get nearest index bucket
    int temp_key = 0;
    while(line_num != 0){
        index_file>>key>>value;
        cout<<key<<" "<<value<<endl;
        if(temp_key >= key){
            value = 0;
            break;
        }
        temp_key = key;
        if(key == ch){
			break;
		}
    }
    cout<<"value is :"<<"key is "<<(char)key<<value;
    int extra_num = num % BLOCK_BYTES;
    bwt_file.seekg(line_num*BLOCK_BYTES, ios::beg);
	char rd_ch;
    for(int i=0; i<extra_num; ++i){
		if(rd_ch == EOF)
			break;
        bwt_file>>noskipws>>rd_ch;
        cout<<endl<<rd_ch<<endl;
        if(rd_ch == ch)
            value++;
    }
    return value;
}
