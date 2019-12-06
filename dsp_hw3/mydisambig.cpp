#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <Ngram.h>
#include <limits>

using namespace std;

struct Node { //(prob,prev)
    double prob;
    string prev;
};

int main(int argc, char** argv){
  /*
  char *seg_path=argv[1];
  char *map_path=argv[2];
  char *mod_path=argv[3];
  char *out_path=argv[4];
  Vocab c;

  vector<string> ZhuYin;
	string tmp;
	for(int i : {116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
	 126, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
	  172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183,
	   184, 185, 186}){
		tmp.push_back(0xA3);
		tmp.push_back(i);
		ZhuYin.push_back(tmp);
		tmp.clear();
	}
  ifstream Zmap(map_path);
  */
  char* map_path=argv[1];
  char* text_path=argv[2];
  char* model_path=argv[3];
  int order = 2;



    Vocab voc;
    Ngram lm( voc, order );
    {
        File lmFile( model_path, "r" );
        lm.read(lmFile);
        lmFile.close();
    }

  vector<string> ZhuYin;
  string tmp;
  for(int i : {116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
   126, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
    172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183,
     184, 185, 186}){
    tmp.push_back(0xA3);
    tmp.push_back(i);
    ZhuYin.push_back(tmp);
    tmp.clear();
  }

  string word;
  vector<string> word_vec;
  ifstream ZYBMF(map_path);
  unordered_map < string, vector<string> > ZYtoB;
  while(getline(ZYBMF,tmp)){
    tmp.push_back(' ');
    for(const string s : ZhuYin){
      if(tmp.substr(0,2) == s){
        for(auto i = tmp.begin(); i!=tmp.end();++i){
          if(*i == ' '){
            word.push_back(*(i-2));
            word.push_back(*(i-1));
            const char *c_word = word.c_str();
            if(VocabIndex wid = voc.getIndex(c_word)!=Vocab_None)
              word_vec.push_back(word);
            word.clear();
          }
        }

        ZYtoB[s] = word_vec;
        word_vec.clear();
      }
    }
  }
  ZYBMF.close();


  ifstream TXT(text_path);
  vector<string> work;
  vector<string> line_out;
  string ww;
  unordered_map<string, Node> Ns;
  vector<unordered_map<string, Node>> Viter;
  vector<string> candi;
  while(getline(TXT, tmp)){

    tmp.push_back(' ');
    line_out.push_back("<s>");
    for(int i = 0; i < tmp.size(); ++i ){
      if(tmp[i]!=' ')
        ww.push_back(tmp[i]);
      if(tmp[i]==' '&&!ww.empty()){
        line_out.push_back(ww);
        ww.clear();
      }
    }
    line_out.push_back("</s>");
    line_out.push_back("dummy");

    int pZYf = -1;
    bool previsZY = 0;
    for(int i = 0; i< line_out.size(); ++i ){
      bool isZY = 0;
      for(string u : ZhuYin){
        if(u.compare(line_out[i])==0){
          if(ZYtoB[u].size()==0){
            line_out[i]="<unk>";
          }else if(ZYtoB[u].size()==1){
            line_out[i]=ZYtoB[u][0];
          }else{
            isZY=1;
          }
          break;
        }
      }
      if((pZYf ==-1) && isZY){
        pZYf = i;
      }else if((pZYf > 0) && ((!isZY) && (!previsZY))){

        for(int j = pZYf-1; j < i; ++j)
          work.push_back(line_out[j]);


        Ns[work[0]] = {0,"dummy"};
        Viter.push_back(Ns);
        Ns.clear();


        for(int j = 1; j < work.size(); ++j){
          if(ZYtoB.find(work[j])!=ZYtoB.end()){
            candi = ZYtoB[work[j]];
          }else{
            candi.push_back(work[j]);
          }
          for(const string sw : candi){
            double NP = numeric_limits<double>::lowest();
            string Nprev = "dummy";
            for(auto &k: Viter.back()){
              const char *w1 = sw.c_str();
              VocabIndex iw1 = voc.getIndex(w1);
                  const char *w2 = k.first.c_str();
                  VocabIndex iw2[] = {voc.getIndex(w2)};

              if(lm.wordProb(iw1, iw2) + k.second.prob > NP){
                NP = lm.wordProb(iw1, iw2) + k.second.prob;
                Nprev = k.first;

              }
            }
            assert(Nprev!="dummy");
            Ns[sw] = {NP, Nprev};
          }
          Viter.push_back(Ns);
          Ns.clear();
          candi.clear();
        }

        --i;
        double maxP = numeric_limits<double>::lowest();
        string Ndprev = "dummy";
        for(auto &j : Viter.back()){
          if(j.second.prob > maxP){
            maxP = j.second.prob;
            Ndprev = j.second.prev;
          }
        }
        while(Viter.size() > 1 ){
          assert(Ndprev != "dummy" );
          --i;
          line_out[i] = Ndprev;
          Viter.pop_back();
          Ndprev = Viter.back()[Ndprev].prev;
        }
        work.clear();
        Viter.clear();
        pZYf = -1;
        previsZY = 0;
        isZY = 0;
      }
      previsZY = isZY;
    }
    line_out.pop_back();
    for(int i = 0; i < (line_out.size()-1) ; ++i)
      cout << line_out[i] << ' ';
    cout << line_out[line_out.size()-1] << '\n';
    line_out.clear();
  }


  return 0;
}
