#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

using namespace std;

void morir(string s)
{
  cout<<s<<endl;
  exit(0);
}

vector<string> leerficherosepararstrings(string nombrefichero)
{
  vector<string> v;
  ifstream ifs;
  ifs.open(nombrefichero.c_str());
  if (ifs.is_open()) {
    string s;
    while (ifs>>s)
      v.push_back(s);
    ifs.close();
  } else {
    morir("Error interno al abrir "+nombrefichero);
  }
  return v;
}

string transforma(string s)
{
  string r;
  for (int i=0;i<int(s.size());i++) {
    if (s[i]!='o')
      r+=string(2,s[i]);
    else
      r+=string(4,s[i]);
  }
  return r;
}

int posicionbarra(string s)
{
  for (int i=0;i<int(s.size());i++)
    if (s[i]=='/')
      return i+1;
  return 0;
}

int main()
{
  string personaje,persona;
  cin>>personaje>>persona;
  string comando="ls audiodibujosanimados/"+persona+"*.wav >listaficheros.txt";
  system(comando.c_str());
  vector<string> v=leerficherosepararstrings("listaficheros.txt");
  for (int i=0;i<int(v.size());i++) {
    string nomaux=v[i].substr(posicionbarra(v[i]));
    cout<<"habla "+personaje+" "+
      transforma(nomaux.substr(int(persona.size())+1,int(nomaux.size())-(int(persona.size())+1+4)))+
      " "+nomaux<<endl<<endl;
  }
}
