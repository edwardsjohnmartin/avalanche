/**
* csvload.h
* Header file designed to provide a functionality for loading a csv
*/

#ifndef CSVLOAD_H
#define CSVLOAD_H

#include <ctype.h>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

//Method designed to load a csv into a vector<vector<string>> table representation
vector<vector<string>> csvLoad(const string &filename) { 
	ifstream fin(filename.c_str());
	if (fin.fail()) {
		return vector<vector<string> >();
	}
	vector<vector<string> > table;

	while (!fin.eof()) {
		string line;
		getline(fin,line);
		if (line == "") {
			continue;
		}

		vector<string> row;
		string tmpword;
		bool quote = false;
		for (int i = 0; i < line.size(); i++) {
			char c = line[i];
			if (c == ',') {
				row.push_back(tmpword);
				tmpword = "";
			} else if (c == '"') {
				quote = !quote;
			} else {
				if (quote || !isspace(c)) {
					tmpword.push_back(c); 
				}
			}
		}
		row.push_back(tmpword);
		table.push_back(row);
	}
	return table;

}	

#endif