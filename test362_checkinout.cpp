/*
 project_362_01.cpp
 ============================= CPSC 362 Project info ===============================
 Project #2 (checkout checkin labeling)
 Due date: 09/28/2018
 Group members:Xinyu Wen, Xianghui Huang, Yintao Wang
 Email: xinyuwen@csu.fullerton.edu, jerryhuang6666@gmail.com, wyt@csu.fullerton.edu
 Class meeting time: Monday & Wednesday 15:00-16:50
 ===================================================================================
 */
#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <io.h>
#include <cstdio>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <map>
#include <vector>

using namespace std;

void copyFile(char * src, char * dest, char * manifest, int cut);
int copyDir(const char * p_src, const char * p_dest, char * manifest, int cut);
string getArtIDFileName(char fileName[]);
void writeFile(char file_name[], const char *message);
void getManifestName(const string cmd, char manifest_name[]);
bool mapToFile(char filename[], map<string, string> &fileMap);
bool fileToMap(char filename[], map<string, string> &fileMap);
void splitString(vector<string> &v_str, string str, char ch);
void addLabel(char filename[]);
vector<string> find_addresses_artID(string file_name);
vector<string> find_addresses_fileName(vector<string> v_addresses);
void check_out(char* src, string manifest, char* dest);
int num_of_manifest;

int main(int argc, char *argv[]) {

	int status = 0;
	char manifest[260], message[260], manifest_path[260];
	//const char * message;
	//const char *command_line = argv[1];
	//const char *src = argv[2];
	//const char *dest = argv[3];
	//status = copyDir(src, dest);
	//status = copyDir("C:\\Users\\yintaowang\\test\\src", "C:\\Users\\yintaowang\\test\\repo");
	//test:
	const char *command_line = "Label";
	char src[260] = "C:\\Users\\yintaowang\\test\\src";
	char dest[260] = "C:\\Users\\yintaowang\\test\\repo";
	char r_manifest[260] = "C:\\Users\\yintaowang\\test\\repo\\manifest_1.txt";


	//get manifest dir
	//char manifest_path[260];
	if (command_line == "Check-Out") {
		strcpy(manifest_path, src);
	}
	else {
		strcpy(manifest_path, dest);
	}

	//manifest file name
	string manifest_p = "dir ";
	manifest_p.append(manifest_path);
	manifest_p.append("\\manifest*.txt /b /a-d | find /v /c \"&#@\"");

	char manifest_name[260];
	getManifestName(manifest_p, manifest_name);
	strcpy(manifest, manifest_path);
	strcat(manifest, "\\");
	strcat(manifest, manifest_name);
	strcpy(message, command_line);
	strcat(message, " ");
	strcat(message, src);
	strcat(message, " ");
	strcat(message, dest);
	time_t now = time(0);

	//label file name
	char label_file[260];
	strcpy(label_file, dest);
	strcat(label_file, "\\label.txt");

	//default label
	string label_default = "";
	//label_default.append(", ");
	//label_default.append(manifest_name);

	if (command_line == "Create") {
		//create manifest file
		writeFile(manifest, message);
		writeFile(manifest, ctime(&now));
		//create label.txt and write default key/value
		writeFile(label_file, label_default.c_str());

		status = copyDir(src, dest, manifest, strlen(dest));
	}
	else if (command_line == "Check-In") {
		//create manifest file
		writeFile(manifest, message);
		writeFile(manifest, ctime(&now));
		//write label.txt
		writeFile(label_file, label_default.c_str());
		status = copyDir(src, dest, manifest, strlen(dest));

		//Check_In(int argc, char *argv[]);
	}
	else if (command_line == "Check-Out") {
		//create manifest file
		writeFile(manifest, message);
		writeFile(manifest, ctime(&now));
		//write label.txt
		writeFile(label_file, label_default.c_str());

		//check out function
		check_out(src, dest, r_manifest, manifest);

	}
	else if (command_line == "Label") {
		//labeling
		addLabel(label_file);
	}
	else {
		cout << "input command is invalid!!!" << endl;
		system("pause");
		return 0;
	}

	//display execution result
	if (status == 0) {
		cout << command_line << " successful." << endl << endl;
	}
	else {
		cout << command_line << " Failed." << endl << endl;
	}
	system("pause");
	return 0;
}



void addLabel(char filename[]) {

	string label, manifest_file;
	string actual_filename;
	map <string, string> manifest_label_map;  //<label, manifest filename>
	bool isValid = false;
	fileToMap(filename, manifest_label_map);

	cout << "which manifest file you want to label?" << endl;
	cin >> manifest_file;

	cout << "What label you want to add to file" << manifest_file << endl;
	cin >> label;


	//check if input is label
	map <string, string> ::iterator itr;
	itr = manifest_label_map.find(manifest_file);
	if (itr == manifest_label_map.end())
		actual_filename = manifest_file;
	else
		actual_filename = itr->second;


	for (int i = 1; i < num_of_manifest; i++) {
		string temp = "manifest_" + to_string(i) + ".txt";
		if (!actual_filename.compare(temp)) {
			isValid = true;
		}
	}
	if (isValid) {
		manifest_label_map.insert(pair <string, string>(label, actual_filename));
		mapToFile(filename, manifest_label_map);
	}
	else {
		cout << "invalid filename" << endl;
	}
}

void writeFile(char file_name[], const char *message) {
	ofstream OutFile;
	OutFile.open(file_name, ios::app);
	OutFile << message << endl;
	OutFile.close();
}

void getManifestName(const string cmd, char manifest_name[]) {

	FILE *crs = _popen(cmd.c_str(), "r");
	char result[1024] = "0";
	fread(result, sizeof(char), sizeof(result), crs);
	if (NULL != crs) {
		fclose(crs);
		crs == NULL;
	}
	int res = stoi(result, nullptr, 10);
	res += 1;
	num_of_manifest = res;
	string temp = "manifest_";
	temp.append(to_string(res));
	temp.append(".txt");
	strcpy(manifest_name, temp.c_str());
}

//copy source file contents to artifact
void copyFile(char * src_file, char * dest_file, char * manifest, int cut) {

	const int buff_size = 16384;
	char buffer[buff_size];
	size_t in_bytes, out_bytes;

	FILE * in_file = fopen(src_file, "rb");
	FILE * out_file = fopen(dest_file, "wb");

	//read source file contents then write into the artifact
	while (!feof(in_file)) {
		in_bytes = fread(buffer, 1, buff_size, in_file);
		out_bytes = fwrite(buffer, 1, in_bytes, out_file);
	}

	fclose(in_file);
	fclose(out_file);
	//write manifest file
	writeFile(manifest, dest_file + cut);
}


//create repository
int copyDir(const char * p_src, const char * p_dest, char * manifest, int cut) {

	if (p_src == NULL || p_dest == NULL) {
		return -1;
	}
	//create a folder
	_mkdir(p_dest);
	char dir[260] = { 0 };
	char src_file_name[260] = { 0 };
	char dest_file_name[260] = { 0 };

	const char * str = "\\*.*";
	strcpy_s(dir, p_src);
	strcat_s(dir, str);

	long hFile;
	_finddata_t fileInfo;

	if ((hFile = _findfirst(dir, &fileInfo)) != -1) {

		do {

			strcpy_s(src_file_name, p_src);
			strcat_s(src_file_name, "\\");
			strcat_s(src_file_name, fileInfo.name);
			strcpy_s(dest_file_name, p_dest);
			strcat_s(dest_file_name, "\\");
			strcat_s(dest_file_name, fileInfo.name);

			//if find a file
			if (!(fileInfo.attrib & _A_SUBDIR)) {

				//create a leaf folder which is named the same as the file name
				_mkdir(dest_file_name);
				//call function to get ArtID file name for the artifact.
				string artId_str = getArtIDFileName(src_file_name);
				string dest_file_name_str = dest_file_name;
				dest_file_name_str = dest_file_name_str + "\\" + artId_str;
				char *dest_file_name_char = const_cast<char *>(dest_file_name_str.c_str());
				//create the artifact of the source file
				copyFile(src_file_name, dest_file_name_char, manifest, cut);

			}
			else if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0) {
				//if find a directory, using the recursion
				copyDir(src_file_name, dest_file_name, manifest, cut);
			}
		} while (_findnext(hFile, &fileInfo) == 0);
		_findclose(hFile);
		return 0;
	}
	return -2;
}

// get ArtID file name
string getArtIDFileName(char fileName[])
{
	string fileNameStr = fileName;
	fstream file;
	string fileContent;
	string artIDFileName;
	string::iterator it;
	string fileTypeName;
	int const FIVEWEIGHTS[] = { 1,3,7,11,17 };
	unsigned long checksum = 0;
	int weightNum;
	int const M = pow(2, 31) - 1;

	//store all chars from a file into a string(including ' ' and '\n')
	file.open(fileNameStr);
	while (file.peek() != EOF) {
		fileContent += file.get();
	}

	//delete all '\n' from the string
	fileContent.erase(std::remove(fileContent.begin(), fileContent.end(), '\n'), fileContent.end());

	//calculate checksum of the string
	for (int i = 0; i < fileContent.length(); i++) {
		weightNum = i % 5;
		checksum += FIVEWEIGHTS[weightNum] * fileContent[i];
	}

	if (checksum < M)
		artIDFileName += std::to_string(checksum);
	else {
		double modulus = (double)checksum / M;
		artIDFileName += std::to_string(modulus);
		artIDFileName += "m";
	}

	//store type name of the file into a string
	for (int i = fileNameStr.length() - 1; i >= 0; i--) {
		fileTypeName = fileTypeName.insert(0, 1, fileNameStr[i]);
		if (fileNameStr[i] == '.')
			break;
	}

	//make a new file name(artid + "-L" + filesize)
	artIDFileName += "-L";
	artIDFileName += std::to_string(fileContent.length());
	artIDFileName += fileTypeName;

	return artIDFileName;
}

bool mapToFile(char filename[], map<string, string> &fileMap)     //Write Map
{
	ofstream ofile;
	ofile.open(filename);
	if (!ofile)
	{
		return false;           //file does not exist and cannot be created.
	}
	map <string, string> ::iterator iter;
	for (iter = fileMap.begin(); iter != fileMap.end(); ++iter)
	{
		ofile << iter->first << "|" << iter->second;
		ofile << "\n";
	}
	return true;
}
bool fileToMap(char filename[], map<string, string> &fileMap)  //Read Map
{
	ifstream ifile;
	ifile.open(filename);
	if (!ifile)
		return false;   //could not read the file.
	string line;
	string key;
	vector<string> v_str;
	while (ifile >> line)
	{
		cout << "line: " << line << endl;
		splitString(v_str, line, '|');

		for (vector<string>::iterator iter = v_str.begin();; ++iter)        //First vector element is the key.
		{
			if (iter == v_str.begin())
			{
				fileMap[*iter] = "Unavailable";
				key = *iter;
				continue;
			}
			fileMap[key] = *iter;
			break;
		}
		v_str.clear();
	}
	return true;
}
void splitString(vector<string> &v_str, string str, char ch)
{
	string sub;
	string::size_type pos = 0;
	string::size_type old_pos = 0;
	bool flag = true;

	while (flag)
	{
		pos = str.find_first_of(ch, pos);
		if (pos == string::npos)
		{
			flag = false;
			pos = str.size();
		}
		sub = str.substr(old_pos, pos - old_pos);  // Disregard the '.'
		v_str.push_back(sub);
		old_pos = ++pos;
	}
}

//void Check_In(int argc, char *argv[])
//{
//	int status = 0;
//	const char * src = argv[1];
//	const char * dest = argv[2];
//	status = copyFile(src, dest, manifest, cut);
//	if (status == 0)
//	{
//		cout << "Successfully Check In!!!" << endl << endl;
//	}
//	else {
//		cout << "Failed to Check In!!!" << endl << endl;
//	}
//}

//extract addresses from a manifest file then store them into a vector
vector<string> find_addresses_artID(string file_name) {
    ifstream file;
    char c;
    string temp;
    vector<string> all_addresses;
    unsigned line_counter = 0;
    
    file.open(file_name);
    while (true) {
        temp = "";
        while (true) {
            c = file.get();
            if (c == '\n' || c == EOF)
                break;
            else
                temp += c;
        }
        //skip the first 3 lines
        if (line_counter < 3) {
            line_counter++;
            continue;
        }
        all_addresses.push_back(temp);
        if (c == EOF)
            break;
    }
    file.close();
    
    return all_addresses;
}

//remove the content after the last '\'
vector<string> find_addresses_fileName(vector<string> v_addresses) {
    vector<string> all_addresses = v_addresses;
    int index_last_backslash;
    string temp_string;
    for(int i = 0; i < all_addresses.size(); i++) {
        for(int j = 0; j < all_addresses[i].length(); j++) {
            if(all_addresses[i][j] == '\\')
                index_last_backslash = j;
        }
        temp_string = all_addresses[i];
        all_addresses[i] = "";
        for(int k = 0; k < j; k++) {
            all_addresses[i] += temp_string[k];
        }
    }
    return all_addresses;
}


void check_out(char* src, char * dest, char* r_manifest, char * w_manifest) {
    vector<string> v_addresses = find_addresses_artID(r_manifest);
    vector<string> v_addresses_fileName = find_addresses_fileName(v_addresses);
    
    for (int i = 0; i < v_addresses.size(); i++) {
        copyFile(src+v_addresses[i], dest+v_addresses_fileName[i], w_manifest, 0);
        //copyFile_noManifest(v_addresses[i], src + dest);
        //writeFile(manifest, v_addresses);
    }
}