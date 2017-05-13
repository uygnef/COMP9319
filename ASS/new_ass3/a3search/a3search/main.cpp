#include <iostream>
#include <array>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include<cassert>
#include<dirent.h>
#include<algorithm>

using namespace std;

#define CREATE_INDEX_DEBUG
//#define WINDOWS

void add_one(map<string, map<string, int>>& index, string word, string file);
void update_index(map<string, map<string, int>>& index, string files);
int create_index(map<string, map<string, int>>& index, string file_list[], int num);

int main(int argc, char* argv[]) {
	bool concept = true;
	if (argv[2] != "-c") {
		concept = false;
	}

#ifdef CREATE_INDEX_DEBUG
	map<string, map<string, int>> debug_index;
	string debug_file[3] = { "file1.txt", "file2.txt", "file3.txt" };
	int num = 3;
	create_index(debug_index, debug_file, 3);

	for (auto it = debug_index.begin(); it != debug_index.end(); ++it)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			cout << it->first << " " << it2->first << " " << it2->second << "\n";
		}
	}
#endif

	pattern = get_pattern(argv[]);
	create_index();
	search(pattern);
	return 0;
}

int create_index(map<string, map<string, int>>& index, string file_list[], int num) {

	for (int i = 0; i < num; ++i) {
		cout << file_list[i] << endl;
		string file = file_list[i];
		update_index(index, file);
	}

}

void update_index(map<string, map<string, int>>& index, string files) {
	fstream file;
	string word;

	file.open(files, fstream::in);
	if (file.fail()) {
		cout << "READ FAIL: " << files << endl;
		return;
	}

	while (file >> boolalpha >> word) {

		add_one(index, word, files);
	}
}

void add_one(map<string, map<string, int>>& index, string word, string file) {
	map<string, map<string, int>>::iterator it;
	map<string, int>::iterator it_2;


	it = index.find(word);
	if (it == index.end()) {

		map<string, int> occur;
		occur.insert(pair<string, int>(file, 1));
		index.insert(pair<string, map<string, int>>(word, occur));
	}
	else {

		it_2 = it->second.find(file);
		if (it_2 == it->second.end()) {
			it->second.insert(pair<string, int>(file, 1));
		}
		else {
			it_2->second += 1;
		}
	}
}

/* Returns a list of files in a directory (except the ones that begin with a dot) */

void GetFilesInDirectory(std::vector<string> &out, const string &directory)
{
#ifdef WINDOWS
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const string file_name = file_data.cFileName;
		const string full_file_name = directory + "/" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
#else
	DIR *dir;
	class dirent *ent;
	class stat st;

	dir = opendir(directory);
	while ((ent = readdir(dir)) != NULL) {
		const string file_name = ent->d_name;
		const string full_file_name = directory + "/" + file_name;

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	}
	closedir(dir);
#endif
} // GetFilesInDirectory