#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

int main(int argc,char **argv){

        char* in_path=argv[1];
        char* out_path=argv[2];
        unordered_map<string,vector<string>> zb_map;
        ifstream infile(in_path);
        string line;
        while (getline(infile,line)){
          cout<<line<<endl;
        }
        cout<<in_path<<endl;
        cout<<out_path<<endl;

        return 0;

}
