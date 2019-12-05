#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#ifndef CHAR_LEN
#define CHAR_LEN 2
#endif

using namespace std;
int main(int argc, char* argv[]){
	char* fin = argv[1];
	char* fout = argv[2];
	unordered_map < string, vector<string> > M;
	string iline;
	ifstream infile(fin);//Big5-ZhuYin.map
	while(getline(infile, iline)){
		vector<string> tmp;
		string wd="", crt="";
		int  i = -1;
		while(iline[++i]!=' ' && iline.begin()+i < iline.end())
			wd.push_back(iline[i]);
		tmp.push_back(wd);
		M[wd] = tmp;
		while(iline.begin()+i < iline.end()){
			if(iline[i]=='/'){
				crt = crt.substr(0,CHAR_LEN);
				bool NotExist = (M.find(crt)== M.end());
				if(NotExist) M[crt] = tmp;
				else{
					if(M[crt].back()!=wd) M[crt].push_back(wd);
				}
				crt.clear();
			}
			if(iline[++i]!=' ') crt.push_back(iline[i]);
		}
		crt = crt.substr(0,CHAR_LEN);
		bool NotExist = (M.find(crt)== M.end());
		if(NotExist) M[crt] = tmp;
		else{
			if(M[crt].back()!=wd) M[crt].push_back(wd);
		}
		tmp.pop_back();
	}
	infile.close();
	#if 0 //terminal debug
	for(const auto &i : M){
		cout << i.first << '\t';
		for(string j : i.second)
			cout << j << ' ';
		cout << '\n';
	}//output Map
	#endif
	#if 1
	char enter[1];
	enter[0] = '\n';
	ofstream outfile(fout);
	for(const auto &i : M){
		outfile << i.first <<'\t';
		for(string j : i.second)
			outfile << j << ' ';
		long pos = outfile.tellp();
    	outfile.seekp(pos - 1); 
		outfile.write(enter,1);
	}
	outfile.close();
	#endif
	return 0;
}
