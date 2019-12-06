#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdlib>
#include <Ngram.h>


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
  char* fmap=NULL;
  char* ftext=NULL;
  char* flm=NULL;
  int order = 0;
  for(int i = 0; i < argc; i++){
    if(argv[i][0] == '-' && argv[i][1] == 't')
      ftext = argv[i+1];
    if(argv[i][0] == '-' && argv[i][1] == 'm')
      fmap = argv[i+1];
    if(argv[i][0] == '-' && argv[i][1] == 'l')
      flm = argv[i+1];
    if(argv[i][0] == '-' && argv[i][1] == 'o')
      order = atoi(argv[i+1]);
  }
  assert(fmap!=NULL&&ftext!=NULL&&flm!=NULL&&order!=0);
  //Bi LM
    Vocab voc;
    Ngram lm( voc, order );
    {
        File lmFile( flm, "r" );
        lm.read(lmFile);
        lmFile.close();
    }
  //ZhuYin map, t= 0
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
  //dictionary, t about 0.21875 sec
  string tmpS;
  vector<string> tmpVS;
  ifstream ZYBMF(fmap);
  unordered_map < string, vector<string> > ZYtoB;
  while(getline(ZYBMF,tmp)){// Read line
    tmp.push_back(' ');
    for(const string s : ZhuYin){
      if(tmp.substr(0,2) == s){ //if it is one of Zhuin
        for(auto i = tmp.begin(); i!=tmp.end();++i){
          if(*i == ' '){
            tmpS.push_back(*(i-2));
            tmpS.push_back(*(i-1));
            const char *c_tmpS = tmpS.c_str();
            if(VocabIndex wid = voc.getIndex(c_tmpS)!=Vocab_None)
              tmpVS.push_back(tmpS);
            tmpS.clear();
          }
        }
        if(tmpVS.empty()){//if none seen in the dictionary
          for(auto i = tmp.begin(); i!=tmp.end();++i){
            if(*i == ' '){//assign random one.
              tmpS.push_back(*(i-2));
              tmpS.push_back(*(i-1));
              tmpVS.push_back(tmpS);
              tmpS.clear();
              break;
            }
          }
        }
        ZYtoB[s] = tmpVS;
        tmpVS.clear();
      }
    }
  }
  ZYBMF.close();
  //readin file and viterbi. t =0.015625s for oneline 0.5s for a file
  //no need to lower_state
  //clock_t tS = clock();
  ifstream TXT(ftext);
  vector<string> work;
  unordered_map<string, Node> Ns;
  vector<unordered_map<string, Node>> Viter;
  vector<string> candi;
  while(getline(TXT, tmp)){
  //getline(TXT, tmp);
    tmp.push_back(' ');
    tmpVS.push_back("<s>");
    for(int i = 0; i < tmp.size(); ++i ){
      if(tmp[i]!=' ')
        tmpS.push_back(tmp[i]);
      if(tmp[i]==' '&&!tmpS.empty()){
        tmpVS.push_back(tmpS);
        tmpS.clear();
      }
    }
    tmpVS.push_back("</s>");//tmpVS formatted
    tmpVS.push_back("dummy");//virtually add one
    //NOW tmpVS is <s> ... zy .... </s> dummy
    int pZYf = -1;
    bool previsZY = 0;
    for(int i = 0; i< tmpVS.size(); ++i ){//not run dummy
      bool isZY = 0;
      for(string u : ZhuYin){//if it is ZY
        if(u.compare(tmpVS[i])==0){
          if(ZYtoB[u].size()==0){
            tmpVS[i]="<unk>";
          }else if(ZYtoB[u].size()==1){
            tmpVS[i]=ZYtoB[u][0];
          }else{
            isZY=1;
          }
          break;
        }
      }
      if((pZYf ==-1) && isZY){
        pZYf = i;
      }else if((pZYf > 0) && ((!isZY) && (!previsZY))){//Do viterbi
        //maybe stack better,but lazy
        for(int j = pZYf-1; j < i; ++j)
          work.push_back(tmpVS[j]);
        //Viterbi work
        //initial
        Ns[work[0]] = {0,"dummy"};
        Viter.push_back(Ns);
        Ns.clear();
        //trans
        for(int j = 1; j < work.size(); ++j){
          if(ZYtoB.find(work[j])!=ZYtoB.end()){
            candi = ZYtoB[work[j]];
          }else{
            candi.push_back(work[j]);
          }
          for(const string sw : candi){//now
            double NP = numeric_limits<double>::lowest();
            string Nprev = "dummy";
            for(auto &k: Viter.back()){//prev
              const char *w1 = sw.c_str();//now
              VocabIndex iw1 = voc.getIndex(w1);
                  const char *w2 = k.first.c_str();
                  VocabIndex iw2[] = {voc.getIndex(w2)};
                  //cout << k.first << ' '<< sw << ' '<< lm.wordProb(iw1, iw2) <<endl;
              if(lm.wordProb(iw1, iw2) + k.second.prob > NP){
                NP = lm.wordProb(iw1, iw2) + k.second.prob;
                Nprev = k.first;
                //cout << '\t' << NP <<endl;
              }
            }
            assert(Nprev!="dummy");
            Ns[sw] = {NP, Nprev};
          }
          Viter.push_back(Ns);
          Ns.clear();
          candi.clear();
        }
        //Terminate and replace
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
          tmpVS[i] = Ndprev;
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
    tmpVS.pop_back();
    for(int i = 0; i < (tmpVS.size()-1) ; ++i)
      cout << tmpVS[i] << ' ';
    cout << tmpVS[tmpVS.size()-1] << '\n';
    tmpVS.clear();
  }


  return 0;
}
