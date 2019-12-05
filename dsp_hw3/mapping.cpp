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
        unordered_map<int,int> _map;
        ifstream infile(in_path);
        string line;

        while (getline(infile,line)){
          vector<string> tmp;
          string w,c="","";

          wd.push_back(line[0]);
          wd.push_back(line[1]);
          tmp.push_back(wd);

          int i = -1;
          while(iline.begin()+i < iline.end()){
            i++;
      			if(line[i]=='/' || line[i]==' '){
      				c = line.substr(i+1,i+3);
      				bool NotExist = (zb_map.find(c)== zb_map.end());
      				if(NotExist) zb_map[c] = tmp;
      				else{
      					if(zb_map[c].back()!=w) zb_map[c].push_back(w);
      				}

      			}

      		}
          tmp.pop_back();

        }
        char enter[1];
        enter[0] = '\n';
        ofstream outfile(out_path);
        for(const auto &i : zb_map){
          outfile << i.first <<'\t';
          for(string j : i.second)
            outfile << j << ' ';
          long pos = outfile.tellp();
            outfile.seekp(pos - 1);
          outfile.write(enter,1);
        }
        outfile.close();


        cout<<in_path<<endl;
        cout<<out_path<<endl;

        return 0;

}
