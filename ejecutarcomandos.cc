#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

const int SCRWIDTH  = 800;
const int SCRHEIGHT = 600;


//#include <allegro.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stack>

#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <cmath>

using namespace std;

bool grabando=false;
bool haybotones=true;
int desplazamientoaudio=0;
int anchowindow,altowindow;
sf::RenderWindow window;
sf::RenderTexture renderTexture;

struct infodibujo
{
  //BITMAP *b;
  sf::Texture t;
  sf::Sprite b;
  int x,y,width,height;
  //int media;
  //int elevacion;
};

struct infoaudio
{
  sf::SoundBuffer b;
};

map<string,infodibujo> dibujo;
map<string,infoaudio> audio;

// Pasar de entero a string.
string itosfichero(int x)
{
  string s;
  if (x==0) s="0";
  else {
    while(x>0) {
      s=string(1,char((x%10)+'0'))+s;
      x=x/10;
    }
  }
  s=string(5-int(s.size()),'0')+s;
  return s;
}

void morir(string s)
{
  cout<<s<<endl;
  //acquire_screen();
  //textout_ex(screen,font,s.c_str(),50,50,
  //	     makecol(255,0,0),makecol(0,0,0));
  //release_screen();
  //rest(4000);
  exit(0);
}

infodibujo &obtenerdibujo(string s)
{
  if (dibujo.count(s)==0) morir("no existe el dibujo: "+s);
  return dibujo[s];
}

sf::SoundBuffer &obteneraudiobuffer(string s)
{
  if (audio.count(s)==0) morir("no existe el audio: "+s);
  return audio[s].b;
}




//char listaficheros[][80]={"ojogrande","cejacae","cejav","cejaa","ceja","pelorubio","pupilaazulderecha","pupilaazulizquierda","pupilaazul",
//	"ojonormal","nariz","bocae","bocaa","bocao","bocau","bocatriste","bocaalegre","bocam","cabeza",""};

/*
void cargarlistaletras(char lista[][10])
{
  for (int i=0;string(lista[i])!="";i++) 
	dibujo[string("boca")+lista[i]].b=obtenerdibujo(string("boca")+lista[0]);
}

char listaa[][10]={"a",""};
char listae[][10]={"e","q","r","t","i","s","d","f","g","h","j","k","l","z","x","c","n",""};
char listao[][10]={"o","w",""};
char listau[][10]={"u","y",""};
char listam[][10]={"m","p","v","b",""};
*/
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

string leelineafichero(string filename)
{
  string contenido;
  ifstream fcontenido(filename.c_str());
  if (not fcontenido.is_open())
    morir("Error interno al abrir '"+filename+"'.");
  string s;
  getline(fcontenido,contenido);
  fcontenido.close();
  return contenido;
}

int posicionbarra(string s)
{
  for (int i=0;i<int(s.size());i++)
    if (s[i]=='/')
      return i+1;
  return 0;
}

sf::Font font;

void cargarfont()
{
  if (!font.loadFromFile("font.otf")) {
    cout<<"no carrega la font"<<endl;
    exit(0);
  }
}

void cargargraficossoporte()
{
  system("ls graficossoporte/* >listaficheros.txt");
  vector<string> v=leerficherosepararstrings("listaficheros.txt");
  for (int i=0;i<int(v.size());i++) {
    string nomaux=v[i];
    cout<<"cargando "<<i<<" "<<nomaux<<endl;
    string nomsinpng=nomaux.substr(posicionbarra(nomaux));
    nomsinpng=nomsinpng.substr(0,int(nomsinpng.size())-4);
    if (!dibujo[nomsinpng].t.loadFromFile(nomaux.c_str()))
      morir("Error al leer "+nomaux);
    dibujo[nomsinpng].x=0;
    dibujo[nomsinpng].y=0;
    dibujo[nomsinpng].width=dibujo[nomsinpng].t.getSize().x;
    dibujo[nomsinpng].height=dibujo[nomsinpng].t.getSize().y;
    dibujo[nomsinpng].b.setTexture(dibujo[nomsinpng].t);
  }
}

void cargardibujos()
{
  sf::Image imagen;
  system("ls graficosdibujosanimados/* >listaficheros.txt");
  vector<string> v=leerficherosepararstrings("listaficheros.txt");
  for (int i=0;i<int(v.size());i++) {
    string nomaux=v[i];
    cout<<"cargando "<<i<<" "<<nomaux<<endl;
    string nomsinpng=nomaux.substr(posicionbarra(nomaux));
    nomsinpng=nomsinpng.substr(0,int(nomsinpng.size())-4);
    if (!imagen.loadFromFile(nomaux.c_str()))
      morir("Error al leer "+nomaux);
    int xminimo=imagen.getSize().x;
    int yminimo=imagen.getSize().y;
    int xmaximo=0,ymaximo=0;
    for (int x=0;x<int(imagen.getSize().x);x++) {
      for (int y=0;y<int(imagen.getSize().y);y++) {
	if (imagen.getPixel(x,y).a!=0) {
	  xminimo=min(xminimo,x);
	  yminimo=min(yminimo,y);
	  xmaximo=max(xmaximo,x);
	  ymaximo=max(ymaximo,y);
	}
      }
    }
    if (xmaximo<xminimo or ymaximo<yminimo)
      morir("Error dibujo "+nomaux+" sin contenido");
    dibujo[nomsinpng].x=xminimo;
    dibujo[nomsinpng].y=yminimo;
    dibujo[nomsinpng].width=xmaximo-xminimo+1;
    dibujo[nomsinpng].height=ymaximo-yminimo+1;
    cout<<nomsinpng<<" "<<xminimo<<" "<<yminimo<<" "<<xmaximo-xminimo+1<<" "<<ymaximo-yminimo+1<<endl;
    //if (!dibujo[nomsinpng].t.loadFromImage(imagen,sf::IntRect(0,0,imagen.getSize().x,imagen.getSize().y)))
    if (!dibujo[nomsinpng].t.loadFromImage(imagen,sf::IntRect(xminimo,yminimo,xmaximo-xminimo+1,ymaximo-yminimo+1)))
      morir("Error al traspasar "+nomaux);
    dibujo[nomsinpng].b.setTexture(dibujo[nomsinpng].t);
  }
}

void cargaraudio()
{
  system("ls audiodibujosanimados/* >listaficheros.txt");
  vector<string> v=leerficherosepararstrings("listaficheros.txt");
  //for (int i=0;string(listaficheros[i])!="";i++) {
  //string nomaux=string(listaficheros[i])+".bmp";
  for (int i=0;i<int(v.size());i++) {
    string nomaux=v[i].substr(posicionbarra(v[i]));
    if (!audio[nomaux].b.loadFromFile(v[i].c_str()))
      morir("Error al leer "+nomaux);
  }
}

int framespersecond=30;

int mystoi(string s)
{
  istringstream ci(s);
  int x;
  ci>>x;
  return x;
  /*
  string aux;
  ci>>aux;
  s=aux;
  int signo=1;
  if (int(s.size())>0 && s[0]=='-') {
    signo=-1;
    s=s.substr(1);
  }
  int r=0;
  for (int i=0;i<int(s.size());i++)
    r=r*10+s[i]-'0';
  return signo*r;
  */
}

string itos(int x)
{
  string signo="";
  if (x<0) {
    x=-x;
    signo="-";
  }
  string s(1,char(x%10+'0'));
  while (x>=10) {
    x/=10;
    s=string(1,char(x%10+'0'))+s;
  }
  return signo+s;
}

string itos5(int x)
{
  string s(5,'0');
  int is=4;
  while (x) {
    if (is<0) {
      cout<<"ERROR itos5"<<endl;
      exit(0);
    }
    s[is]=char('0'+x%10);
    is--;
    x/=10;
  }
  cout<<s<<endl;
  return s;
}

bool esnatural(string s)
{
  if (int(s.size())==0) return false;
  for (int i=0;i<int(s.size());i++)
    if (! (s[i]>='0' && s[i]<='9'))
      return false;
  return true;
}

bool esentero(string s)
{
  if (int(s.size())==0) return false;
  if (s[0]=='-') return esnatural(s.substr(1));
  return esnatural(s);
}

void morir(int linea,int columna,string s)
{
  morir("Error linea "+itos(linea+1)+" columna "+itos(columna+1)+": "+s);
}

vector<string> leerfichero(string nombrefichero)
{
  vector<string> v;
  ifstream ifs;
  ifs.open(nombrefichero.c_str());
  if (ifs.is_open()) {
    string s;
    while (getline(ifs,s)) {
      for (int i=0;i<int(s.size());i++)
	if (s[i]==13 || s[i]=='\t') s[i]=' ';
      v.push_back(s);
    }
    ifs.close();
  } else {
    morir("Error interno al abrir "+nombrefichero);
  }
  return v;
}

void saltarblancos(string &s,int &is)
{
  while (is<int(s.size()) && s[is]==' ') is++;
}

void eliminacomentarios(string &s)
{
  int is=0;
  while (is<int(s.size())-1 && !(s[is]=='/' && s[is+1]=='/')) is++;
  if (is<int(s.size())-1) s=s.substr(0,is);
}

void eliminacomentarios(vector<string> &vs)
{
  for (int i=0;i<int(vs.size());i++)
    eliminacomentarios(vs[i]);
}

vector<pair<int,string> > s2vis(string s)
{
  eliminacomentarios(s);
  vector<pair<int,string> > vis;
  int is=0;
  saltarblancos(s,is);
  while (is<int(s.size())) {
    int antis=is;
    while (is<int(s.size()) && s[is]!=' ') is++;
    vis.push_back(pair<int,string> (antis,s.substr(antis,is-antis)));
    saltarblancos(s,is);
  }
  return vis;
}

vector<vector<pair<int,string> > > vs2vvis(vector<string> &vs)
{
  vector<vector<pair<int,string> > > vvis;
  for (int i=0;i<int(vs.size());i++)
    vvis.push_back(s2vis(vs[i]));
  return vvis;
}

void escribe(vector<vector<pair<int,string> > > &vvis)
{
  for (int i=0;i<int(vvis.size());i++) {
    for (int j=0;j<int(vvis[i].size());j++)
      cout<<"("<<vvis[i][j].first<<","<<vvis[i][j].second<<")";
    cout<<endl;
  }
}

void escribe(vector<vector<string> > &vvs)
{
  for (int i=0;i<int(vvs.size());i++) {
    for (int j=0;j<int(vvs[i].size());j++)
      cout<<vvs[i][j]<<",";
    cout<<endl;
  }
}

void push_back_instruccion(vector<pair<int,string> > &vis,vector<vector<string> > &vvs)
{
  vector<string> vs;
  for (int i=0;i<int(vis.size());i++)
    vs.push_back(vis[i].second);
  vvs.push_back(vs);
}

void transformainstruccion(int linea,vector<pair<int,string> > &vis,vector<vector<string> > &vvs)
{
  if (int(vis.size())==0) return;
  int columna=vis[0].first;
  if (vis[0].second=="letras") {
    if (int(vis.size())!=2)
      morir(linea,columna,vis[0].second+" requiere 'listaletras'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="coloca" || vis[0].second=="centra" || vis[0].second=="escala") {
    if (int(vis.size())!=4 || ! esentero(vis[2].second) || ! esentero(vis[3].second))
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'xfin' 'yfin'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="elimina") {
    if (int(vis.size())!=2)
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="transicionini" or vis[0].second=="transicionter" or vis[0].second=="transicionfin") {
    if (int(vis.size())!=2)
      morir(linea,columna,vis[0].second+" requiere 'idtransicion'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="obviar" or vis[0].second=="instantaneo"
	     or vis[0].second=="finobviar" or vis[0].second=="fininstantaneo") {
    if (int(vis.size())!=1)
      morir(linea,columna,vis[0].second+" no requiere parametros.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="quita") {
    if (int(vis.size())!=3)
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'idcaracteristica'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="prefijoboca") {
    if (int(vis.size())!=3)
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'stringprefijoboca'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="habla") {
    if (int(vis.size())!=4 || esnatural(vis[2].second))
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'frase' ('tiempo'||'ficheroaudio').");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="mueve" || vis[0].second=="mueveconcamara") {
    if (int(vis.size())!=5 || ! esentero(vis[2].second) || ! esentero(vis[3].second) ||
	! esnatural(vis[4].second))
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'xfin' 'yfin' 'tiempo'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="camara") {
    if (int(vis.size())!=5 || ! esentero(vis[1].second) || ! esentero(vis[2].second) ||
	! esnatural(vis[3].second) || ! esnatural(vis[4].second))
      morir(linea,columna,vis[0].second+" requiere 'xini' 'yini' 'ancho' 'alto'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="muevecamara") {
    if (int(vis.size())!=6 || ! esentero(vis[1].second) || ! esentero(vis[2].second) ||
	! esnatural(vis[3].second) || ! esnatural(vis[4].second) || ! esnatural(vis[5].second))
      morir(linea,columna,vis[0].second+" requiere 'xini' 'yini' 'ancho' 'alto' 'tiempo'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="luz") {
    if (int(vis.size())!=2 || ! esnatural(vis[1].second) || mystoi(vis[1].second)>=256)
      morir(linea,columna,vis[0].second+" requiere 'iluminacion<256'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="mueveluz") {//cambiar a mueveluz
    if (int(vis.size())!=3 || ! esnatural(vis[1].second) ||
	mystoi(vis[1].second)>=256 || ! esnatural(vis[2].second))
      morir(linea,columna,vis[0].second+" requiere 'iluminacionfinal<256' 'tiempo'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="definecolor") {
    if (int(vis.size())!=5 || ! esnatural(vis[2].second) || ! esnatural(vis[3].second) ||
	! esnatural(vis[4].second) || mystoi(vis[2].second)>=256 || mystoi(vis[3].second)>=256
	|| mystoi(vis[4].second)>=256)
      morir(linea,columna,vis[0].second+" requiere 'idcolor' 'R<256' 'G<256' 'B<256'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="color") {
    if (int(vis.size())!=4)
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'idcaracteristica' 'idcolor'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="forma") {
    if (int(vis.size())!=4)
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'idcaracteristica' 'forma'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="fps") {
    if (int(vis.size())!=2 || ! esnatural(vis[1].second))
      morir(linea,columna,vis[0].second+" requiere 'framespersecond'.");
    framespersecond=mystoi(vis[1].second);
  } else if (vis[0].second=="ventana") {
    if (int(vis.size())!=3 || ! esnatural(vis[1].second) || ! esnatural(vis[2].second))
      morir(linea,columna,vis[0].second+" requiere 'ancho' 'alto'.");
    //window.setSize(sf::Vector2u(mystoi(vis[1].second),mystoi(vis[2].second)));
    anchowindow=mystoi(vis[1].second);
    altowindow=mystoi(vis[2].second);
  } else if (vis[0].second=="profundidad") {
    if (int(vis.size())!=4 || ! esentero(vis[3].second))
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje' 'idcaracteristica' 'profundidad(entero)'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="flip") {
    if (int(vis.size())!=2)
      morir(linea,columna,vis[0].second+" requiere 'idpersonaje'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="tiempo") {
    if (int(vis.size())!=2 || ! esnatural(vis[1].second))
      morir(linea,columna,vis[0].second+" requiere 'tiempo'.");
    push_back_instruccion(vis,vvs);
  } else if (vis[0].second=="sonido") {
    if (int(vis.size())!=2)
      morir(linea,columna,vis[0].second+" requiere 'ficheroaudio'.");
    push_back_instruccion(vis,vvs);
  } else
    morir(linea,columna,"'"+vis[0].second+"' no es una instruccion valida.");
}

vector<vector<string> > transformainstrucciones(vector<vector<pair<int,string> > > &vvis)
{
  vector<vector<string> > vvs;
  for (int i=0;i<int(vvis.size());i++)
    transformainstruccion(i,vvis[i],vvs);
  return vvs;
}

void transformaaframes(string &s)
{
  // ERROR ARREGLADO. ANTES ERA: s=itos(mystoi(s)/framespersecond);
  s=itos(mystoi(s)*framespersecond/1000);
}

void transformaaframes(vector<string> &vs)
{
  if (vs[0]=="habla" and esnatural(vs[3])) transformaaframes(vs[3]);
  else if (vs[0]=="mueve" || vs[0]=="mueveconcamara") transformaaframes(vs[4]);
  else if (vs[0]=="muevecamara") transformaaframes(vs[5]);
  else if (vs[0]=="tiempo") transformaaframes(vs[1]);
  else if (vs[0]=="mueveluz") transformaaframes(vs[2]);
}

void transformaaframes(vector<vector<string> > &vvs)
{
  for (int i=0;i<int(vvs.size());i++)
    transformaaframes(vvs[i]);
}

// El estado tiene unas coordenadas de camara,
// y un map de personajes a un struct con un map
// de nombre de caracteristica a forma, y un map de nombre de caracteristica
// a color.

struct estadopersonaje {
  int x,y;
  int xcentro,ycentro;
  int xescala,yescala;
  int flip;
  map<string,string> caracteristica2forma;
  map<string,int> caracteristica2profundidad;
  map<string,int> caracteristica2color;
  string prefijoboca;
  estadopersonaje() {
    x=y=xcentro=ycentro=flip=0;
    xescala=yescala=100;
    prefijoboca="boca";
  }
};

struct estadogeneral {
  int xcamara,ycamara,anchocamara,altocamara,luz;
  map<string,estadopersonaje> p;
  int accionaudio;// 0=nada, 1=iniciaraudio, 2=oyendose.
  string nombreaudio;
  float tantoaudio; // tanto por uno de audio ya escuchado del audio que esta oyendose.
  estadogeneral() {
    xcamara=ycamara=0;
    anchocamara=1000;
    altocamara=700;
    luz=255;
    accionaudio=0;
  }
};

void escribe(estadopersonaje &p)
{
  cout<<"x: "<<p.x<<endl;
  cout<<"y: "<<p.y<<endl;
  cout<<"flip: "<<p.flip<<endl;
  for (map<string,string>::iterator it=p.caracteristica2forma.begin();it!=p.caracteristica2forma.end();it++)
    cout<<"("<<it->first<<","<<it->second<<")";
  cout<<endl;
  for (map<string,int>::iterator it=p.caracteristica2profundidad.begin();it!=p.caracteristica2profundidad.end();it++)
    cout<<"("<<it->first<<","<<it->second<<")";
  cout<<endl;
}

void escribe(estadogeneral &e)
{
  cout<<"----------------------------"<<endl;
  cout<<"----------------------------"<<endl;
  cout<<"xcamara:"<<e.xcamara<<endl;
  cout<<"ycamara:"<<e.ycamara<<endl;
  cout<<"anchocamara:"<<e.anchocamara<<endl;
  cout<<"altocamara:"<<e.altocamara<<endl;
  cout<<"luz:"<<e.luz<<endl;
  for (map<string,estadopersonaje>::iterator it=e.p.begin();it!=e.p.end();it++) {
    cout<<"- Personaje "<<it->first<<endl;
    escribe(it->second);
  }
}

void escribe(vector<estadogeneral> &ve)
{
  for (int i=0;i<int(ve.size());i++)
    escribe(ve[i]);
}

void transformaaestados(vector<string> &vs,vector<estadogeneral> &ve,estadogeneral &e,
			map<string,int> &color,map<char,char> &letra2letra,
			//vector<pair<int,string> > &listaaudios,
			bool &instantaneo)
{
  if (vs[0]=="instantaneo") {
    instantaneo=true;
  } else if (vs[0]=="fininstantaneo") {
    instantaneo=false;
  } else if (vs[0]=="sonido") {
    sf::SoundBuffer &soundbuffer=obteneraudiobuffer(vs[1]);
    int numframes=int(soundbuffer.getDuration().asMilliseconds())*framespersecond/1000;
    if (not instantaneo) {
      //listaaudios.push_back(pair<int,string> (int(ve.size()),vs[1]));
      e.nombreaudio=vs[1];
      for (int i=0;i<numframes;i++) {
	if (i==0) {
	  e.accionaudio=1;
	  e.tantoaudio=0;
	} else if (i<numframes-2) {
	  e.accionaudio=2;
	  e.tantoaudio=float(i)/(numframes-1);	  
	} else {
	  e.accionaudio=0;
	}
	ve.push_back(e);
      }
      e.accionaudio=0;
    }
  } else if (vs[0]=="habla") {
    estadopersonaje &p=e.p[vs[1]];
    string frase=vs[2];
    int numframes;
    bool hayaudio=false;
    if (esnatural(vs[3]))
      numframes=mystoi(vs[3]);
    else {
      sf::SoundBuffer soundbuffer=obteneraudiobuffer(vs[3]);
      numframes=int(soundbuffer.getDuration().asMilliseconds())*framespersecond/1000;
      hayaudio=true;
      //if (not instantaneo)
      //listaaudios.push_back(pair<int,string> (int(ve.size()),vs[3]));
    }
    string &forma=p.caracteristica2forma[p.prefijoboca];
    string formaanterior=forma;
    if (forma=="" || int(forma.size())>1) forma="a";
    char &c=forma[int(forma.size())-1];
    if (hayaudio)
      e.nombreaudio=vs[3];
    if (instantaneo or numframes<=0) {
      c=letra2letra[frase[int(frase.size())-1]];
    } else if (numframes==1) {
      c=letra2letra[frase[int(frase.size())-1]];
      if (hayaudio)
	e.accionaudio=1;
      ve.push_back(e);
      if (hayaudio)
	e.accionaudio=0;
    } else {
      for (int frame=0;frame<numframes;frame++) {
	c=letra2letra[frase[int(frase.size())*frame/numframes]];
	if (hayaudio) {
	  if (frame==0) {
	    e.accionaudio=1;
	    e.tantoaudio=0;
	  } else if (frame<numframes-2) {
	    e.accionaudio=2;
	    e.tantoaudio=float(frame)/(numframes-1);	  
	  } else {
	    e.accionaudio=0;
	  }
	}
	ve.push_back(e);
      }
      c=letra2letra[frase[int(frase.size())-1]];
      if (hayaudio)
	e.accionaudio=0;
    }
    forma=formaanterior;
  } else if (vs[0]=="mueve" || vs[0]=="mueveconcamara") {
    estadopersonaje &p=e.p[vs[1]];
    int &x=p.x;
    int &y=p.y;
    int xini=x;
    int yini=y;
    int xfin=mystoi(vs[2]);
    int yfin=mystoi(vs[3]);
    int numframes=mystoi(vs[4]);
    int &xcamara=e.xcamara;
    int &ycamara=e.ycamara;
    int despxcamara=xcamara-x;
    int despycamara=ycamara-y;
    if (instantaneo or numframes<=0) {
      x=xfin;
      y=yfin;
      if (vs[0]=="mueveconcamara") {
	xcamara=x+despxcamara;
	ycamara=y+despycamara;
      }
    } else if (numframes==1) {
      x=xfin;
      y=yfin;
      if (vs[0]=="mueveconcamara") {
	xcamara=x+despxcamara;
	ycamara=y+despycamara;
      }
      ve.push_back(e);
    } else {
      for (int frame=0;frame<numframes;frame++) {
	x=(xini*(numframes-1-frame)+xfin*frame)/(numframes-1);
	y=(yini*(numframes-1-frame)+yfin*frame)/(numframes-1);
	if (vs[0]=="mueveconcamara") {
	  xcamara=x+despxcamara;
	  ycamara=y+despycamara;
	}
	ve.push_back(e);
      }
    }
  } else if (vs[0]=="muevecamara") {
    int &x=e.xcamara;
    int &y=e.ycamara;
    int &ancho=e.anchocamara;
    int &alto=e.altocamara;
    int xini=x;
    int yini=y;
    int anchoini=ancho;
    int altoini=alto;
    int xfin=mystoi(vs[1]);
    int yfin=mystoi(vs[2]);
    int anchofin=mystoi(vs[3]);
    int altofin=mystoi(vs[4]);
    int numframes=mystoi(vs[5]);
    if (instantaneo or numframes<=0) {
      x=xfin;
      y=yfin;
    } else if (numframes==1) {
      x=xfin;
      y=yfin;
      ve.push_back(e);
    } else {
      for (int frame=0;frame<numframes;frame++) {
	x=(xini*(numframes-1-frame)+xfin*frame)/(numframes-1);
	y=(yini*(numframes-1-frame)+yfin*frame)/(numframes-1);
	ancho=(anchoini*(numframes-1-frame)+anchofin*frame)/(numframes-1);
	alto=(altoini*(numframes-1-frame)+altofin*frame)/(numframes-1);
	ve.push_back(e);
      }
    }
  } else if (vs[0]=="coloca") {
    estadopersonaje &p=e.p[vs[1]];
    p.x=mystoi(vs[2]);
    p.y=mystoi(vs[3]);
  } else if (vs[0]=="centra") {
    estadopersonaje &p=e.p[vs[1]];
    p.xcentro=mystoi(vs[2]);
    p.ycentro=mystoi(vs[3]);
  } else if (vs[0]=="escala") {
    estadopersonaje &p=e.p[vs[1]];
    p.xescala=mystoi(vs[2]);
    p.yescala=mystoi(vs[3]);
  } else if (vs[0]=="elimina") {
    e.p.erase(vs[1]);
  } else if (vs[0]=="quita") {
    estadopersonaje &p=e.p[vs[1]];
    p.caracteristica2forma.erase(vs[2]);
  } else if (vs[0]=="prefijoboca") {
    estadopersonaje &p=e.p[vs[1]];
    p.prefijoboca=vs[2];
  } else if (vs[0]=="luz") {
    e.luz=mystoi(vs[1]);
  } else if (vs[0]=="mueveluz") {
    int luzini=e.luz;
    int luzfin=mystoi(vs[1]);
    int numframes=mystoi(vs[2]);
    if (instantaneo or numframes<=0) {
      e.luz=luzfin;
    } else if (numframes==1) {
      e.luz=luzfin;
      ve.push_back(e);
    } else {
      for (int frame=0;frame<numframes;frame++) {
	e.luz=(luzini*(numframes-1-frame)+luzfin*frame)/(numframes-1);
	ve.push_back(e);
      }
    }
  } else if (vs[0]=="tiempo") {
    if (not instantaneo) {
      int numframes=mystoi(vs[1]);
      for (int i=0;i<numframes;i++)
	ve.push_back(e);
    }
  } else if (vs[0]=="letras") {
    string &letras=vs[1];
    for (int i=0;i<int(letras.size());i++)
      letra2letra[letras[i]]=letras[0];
  } else if (vs[0]=="camara") {
    e.xcamara=mystoi(vs[1]);
    e.ycamara=mystoi(vs[2]);
    e.anchocamara=mystoi(vs[3]);
    e.altocamara=mystoi(vs[4]);
  } else if (vs[0]=="definecolor") {
    //color[vs[1]]=makecol(mystoi(vs[2]),mystoi(vs[3]),mystoi(vs[4]));
  } else if (vs[0]=="color") {
    e.p[vs[1]].caracteristica2color[vs[2]]=color[vs[3]];
  } else if (vs[0]=="forma") {
    e.p[vs[1]].caracteristica2forma[vs[2]]=vs[3];
  } else if (vs[0]=="profundidad") {
    e.p[vs[1]].caracteristica2profundidad[vs[2]]=mystoi(vs[3]);
  } else if (vs[0]=="flip") {
    e.p[vs[1]].flip^=1;
  } else {
    cout<<"Instruccion inesperada "<<vs[0]<<"."<<endl;
    exit(0);
  }
}

void transformaaestados(vector<vector<string> > &vvs,vector<estadogeneral> &ve)
//vector<pair<int,string> > &listaaudios)
{
  estadogeneral e;
  map<string,int> frametransicion;
  e.xcamara=anchowindow/2;//window.getSize().x/2;
  e.ycamara=altowindow/2;//window.getSize().y/2;
  e.anchocamara=anchowindow;//window.getSize().x;
  e.altocamara=altowindow;//window.getSize().y;
  bool instantaneo=false;
  map<string,int> color;
  map<char,char> letra2letra;
  for (int i=0;i<int(vvs.size());i++) {
    if (vvs[i][0]=="obviar")
      while (i<int(vvs.size()) and vvs[i][0]!="finobviar")
	i++;
    else if (vvs[i][0]=="transicionini") {
      string idtransicion=vvs[i][1];
      if (frametransicion.count(idtransicion)) {
	cout<<"Transicion "<<idtransicion<<" se definio dos veces."<<endl;
	exit(0);
      }
      frametransicion[idtransicion]=int(ve.size());
    } else if (vvs[i][0]=="transicionter") {
      string idtransicion=vvs[i][1];
      if (frametransicion.count(idtransicion)==0) {
	cout<<"Transicion "<<idtransicion<<" no definida."<<endl;
	exit(0);
      }
      int frameinitransicion=frametransicion[idtransicion];
      int framefintransicion=int(ve.size())-1;
      i++;
      int jini=i;
      while (i<int(vvs.size()) and vvs[i][0]!="transicionfin") i++;
      if (i>=int(vvs.size()) or vvs[i][1]!=idtransicion) {
	cout<<"Transicion "<<idtransicion<<" sin marca de final."<<endl;
	exit(0);
      }
      int jfin=i-1;
      for (int frame=frameinitransicion;frame<=framefintransicion;frame++) {
	double factor0=0,factor1=1;
	if (frame<framefintransicion) {
	  factor0=double(framefintransicion-frame)/(framefintransicion-frameinitransicion);
	  factor1=double(frame-frameinitransicion)/(framefintransicion-frameinitransicion);
	}
	for (int j=jini;j<=jfin;j++) {
	  vector<string> &vs=vvs[j];
	  if (vs[0]=="camara") {
	    int xcamaraini=ve[frameinitransicion].xcamara;
	    int ycamaraini=ve[frameinitransicion].ycamara;
	    int anchocamaraini=ve[frameinitransicion].anchocamara;
	    int altocamaraini=ve[frameinitransicion].altocamara;
	    e.xcamara=mystoi(vs[1]);
	    e.ycamara=mystoi(vs[2]);
	    e.anchocamara=mystoi(vs[3]);
	    e.altocamara=mystoi(vs[4]);
	    ve[frame].xcamara=xcamaraini*factor0+e.xcamara*factor1;
	    ve[frame].ycamara=ycamaraini*factor0+e.ycamara*factor1;
	    ve[frame].anchocamara=anchocamaraini*factor0+e.anchocamara*factor1;
	    ve[frame].altocamara=altocamaraini*factor0+e.altocamara*factor1;
	  } else if (vs[0]=="coloca") {
	    int xini=ve[frameinitransicion].p[vs[1]].x;
	    int yini=ve[frameinitransicion].p[vs[1]].y;
	    estadopersonaje &p=e.p[vs[1]];
	    p.x=mystoi(vs[2]);
	    p.y=mystoi(vs[3]);
	    ve[frame].p[vs[1]].x=xini*factor0+p.x*factor1;
	    ve[frame].p[vs[1]].y=yini*factor0+p.y*factor1;
	  } else {
	    cout<<"Instruccion "<<vs[0]<<" no permitida en definicion de transicion "<<idtransicion<<"."<<endl;
	    exit(0);
	  }
	}
      }
    } else
      transformaaestados(vvs[i],ve,e,color,letra2letra/*,listaaudios*/,instantaneo);
  }
}

// El estado se convertira en un primer paso en una lista de planes de dibujo.
// Estos planes de dibujo se ordenan por profundidad. Despues, se dibujan por orden.
// Finalmente, se usan xcamara,ycamara para pillar la parte que interesa.

struct plandibujo {
  int x,y;
  int xcentro,ycentro;
  int xescala,yescala;
  int flip;
  int profundidad;
  int color;
  string dibujo;
  plandibujo() {
  }
  plandibujo(int inx,int iny,int inxcentro,int inycentro,int inxescala,int inyescala,int inflip,
	     int inprofundidad,int incolor,string indibujo) {
    x=inx;y=iny;xcentro=inxcentro;ycentro=inycentro;xescala=inxescala;yescala=inyescala;flip=inflip;
    profundidad=inprofundidad;color=incolor;dibujo=indibujo;
  }
};

void escribe(plandibujo &p)
{
  cout<<"x: "<<p.x<<endl;
  cout<<"y: "<<p.y<<endl;
  cout<<"flip: "<<p.flip<<endl;
  cout<<"profundidad: "<<p.profundidad<<endl;
  cout<<"color: "<<p.color<<endl;
  cout<<"dibujo: "<<p.dibujo<<endl;
}

void escribe(vector<plandibujo> &vp)
{
  for (int i=0;i<int(vp.size());i++)
    escribe(vp[i]);
}

bool operator<(plandibujo d1,plandibujo d2)
{
  return d1.profundidad<d2.profundidad;
}

void transformaaplandibujo(estadopersonaje &p,vector<plandibujo> &vp)
{
  for (map<string,string>::iterator it=p.caracteristica2forma.begin();it!=p.caracteristica2forma.end();it++)
    vp.push_back(plandibujo(p.x,p.y,p.xcentro,p.ycentro,p.xescala,p.yescala,
			    p.flip,p.caracteristica2profundidad[it->first],
			    p.caracteristica2color[it->first],
			    it->first+it->second));
}

vector<plandibujo> transformaaplandibujo(estadogeneral &e)
{
  vector<plandibujo> vp;
  for (map<string,estadopersonaje>::iterator it=e.p.begin();it!=e.p.end();it++)
    transformaaplandibujo(it->second,vp);
  sort(vp.begin(),vp.end());
  return vp;
}

void cargarve(vector<estadogeneral> &ve)//,vector<pair<int,string> > &listaaudios)
{
  ve=vector<estadogeneral> ();
  vector<string> vs=leerfichero("comandos.txt");
  vector<vector<pair<int,string> > > vvis=vs2vvis(vs);
  //escribe(vvis);
  vector<vector<string> > vvs=transformainstrucciones(vvis);
  transformaaframes(vvs);
  //cout<<endl;
  //escribe(vvs);
  transformaaestados(vvs,ve);//,listaaudios);

  if (desplazamientoaudio>0) {
    for (int i=int(ve.size())-1;i>=0;i--) {
      if (i>=desplazamientoaudio) {
	ve[i].accionaudio=ve[i-desplazamientoaudio].accionaudio;
	ve[i].nombreaudio=ve[i-desplazamientoaudio].nombreaudio;
	ve[i].tantoaudio=ve[i-desplazamientoaudio].tantoaudio;
      } else {
	ve[i].accionaudio=0;
	ve[i].nombreaudio="";
	ve[i].tantoaudio=0;
      }
    }
  } else if (desplazamientoaudio<0) {
    desplazamientoaudio*=-1;
    for (int i=0;i<int(ve.size());i++) {
      if (i<int(ve.size())-desplazamientoaudio) {
	ve[i].accionaudio=ve[i+desplazamientoaudio].accionaudio;
	ve[i].nombreaudio=ve[i+desplazamientoaudio].nombreaudio;
	ve[i].tantoaudio=ve[i+desplazamientoaudio].tantoaudio;
      } else {
	ve[i].accionaudio=0;
	ve[i].nombreaudio="";
	ve[i].tantoaudio=0;
      }
    }
    desplazamientoaudio*=-1;
  }
  //escribe(ve);
}



void texturarestadogeneral(estadogeneral &e)
{
  renderTexture.clear(sf::Color::Black);
  vector<plandibujo> vp=transformaaplandibujo(e);
  float xescala=float(renderTexture.getSize().x)/e.anchocamara;
  float yescala=float(renderTexture.getSize().y)/e.altocamara;
  float leftrenderTexture=e.xcamara-e.anchocamara/2.0;
  float toprenderTexture=e.ycamara-e.altocamara/2.0;
  //cout<<"("<<renderTexture.getSize().x<<","<<renderTexture.getSize().y<<")";
  for (int i=0;i<int(vp.size());i++) {
    plandibujo &p=vp[i];
    infodibujo &info=obtenerdibujo(p.dibujo);
    float xdesp=p.x-leftrenderTexture;
    float ydesp=p.y-toprenderTexture;
    info.b.setOrigin(p.xcentro-info.x,p.ycentro-info.y);
    info.b.setPosition(xdesp*xescala,ydesp*yescala);
    info.b.setScale(xescala*p.xescala/100.0,yescala*p.yescala/100.0);
    info.b.setColor(sf::Color(e.luz,e.luz,e.luz,255));
    renderTexture.draw(info.b);
  }

  sf::View view;
  view.setCenter(sf::Vector2f(renderTexture.getSize().x/2.0,renderTexture.getSize().y/2.0));
  view.setSize(sf::Vector2f(renderTexture.getSize().x,renderTexture.getSize().y));
  renderTexture.setView(view);
  
  renderTexture.display();
}

void salvartexturageneral(string nombrefichero)
{
  renderTexture.getTexture().copyToImage().saveToFile(nombrefichero);
}


map<string,sf::IntRect> boton2rect;
float tantoladoboton=0.05;
float ladoboton;

void dibujarestadogeneral(estadogeneral &e,int frame,int totalframes)
{
  //static int ultimoframe=-1;
  //if (frame==ultimoframe) return;
  //ultimoframe=frame;

  window.clear(sf::Color::Black);
  //rectfill(pantalla,0,0,1000,700,makecol(30,0,0));
  vector<plandibujo> vp=transformaaplandibujo(e);
  //cout<<endl<<endl;
  //escribe(vp);
  float xescala=float(window.getSize().x)/e.anchocamara;
  float yescala=float(window.getSize().y)/e.altocamara;
  float leftwindow=e.xcamara-e.anchocamara/2.0;
  float topwindow=e.ycamara-e.altocamara/2.0;
  //cout<<"("<<window.getSize().x<<","<<window.getSize().y<<")";
  for (int i=0;i<int(vp.size());i++) {
    plandibujo &p=vp[i];
    //draw_sprite(pantalla,obtenerdibujo(p.dibujo),p.x,p.y);
    infodibujo &info=obtenerdibujo(p.dibujo);
    //sf::Transform t = sf::Transform::Identity;
    float xdesp=p.x-leftwindow;
    float ydesp=p.y-topwindow;
    info.b.setOrigin(p.xcentro-info.x,p.ycentro-info.y);
    info.b.setPosition(xdesp*xescala,ydesp*yescala);
    //d.setPosition(p.x,p.y);
    info.b.setScale(xescala*p.xescala/100.0,yescala*p.yescala/100.0);
    info.b.setColor(sf::Color(e.luz,e.luz,e.luz,255));
    window.draw(info.b);
  }

  if (haybotones) {
    ladoboton=tantoladoboton*min(window.getSize().x,window.getSize().y);
    boton2rect["play"]=sf::IntRect(ladoboton,window.getSize().y-2*ladoboton,ladoboton,ladoboton);
    boton2rect["pause"]=sf::IntRect(3*ladoboton,window.getSize().y-2*ladoboton,ladoboton,ladoboton);
    boton2rect["stop"]=sf::IntRect(5*ladoboton,window.getSize().y-2*ladoboton,ladoboton,ladoboton);
    boton2rect["rev"]=sf::IntRect(7*ladoboton,window.getSize().y-2*ladoboton,ladoboton,ladoboton);
    boton2rect["barra"]=sf::IntRect(9*ladoboton,window.getSize().y-2*ladoboton,window.getSize().x-10*ladoboton,ladoboton);
    boton2rect["indicador"]=sf::IntRect(float(totalframes-1-frame)/(totalframes-1)*9*ladoboton+
					float(frame)/(totalframes-1)*(window.getSize().x-ladoboton)-ladoboton/10,
					window.getSize().y-2*ladoboton,ladoboton/5,ladoboton);
    boton2rect["slow"]=sf::IntRect(ladoboton,window.getSize().y-4*ladoboton,ladoboton,ladoboton);
    boton2rect["slowstop"]=sf::IntRect(3*ladoboton,window.getSize().y-4*ladoboton,ladoboton,ladoboton);
    boton2rect["slowrev"]=sf::IntRect(5*ladoboton,window.getSize().y-4*ladoboton,ladoboton,ladoboton);
    
    for (map<string,sf::IntRect>::iterator it=boton2rect.begin();it!=boton2rect.end();it++) {
      string nombre=it->first;
      sf::IntRect rect=it->second;
      infodibujo &info=obtenerdibujo("boton"+nombre);
      info.b.setPosition(rect.left,rect.top);
      info.b.setScale(float(rect.width)/info.width,float(rect.height)/info.height);
      window.draw(info.b);
    }
    
    if (e.accionaudio==1 or e.accionaudio==2) {
      sf::Text text;
      text.setString(e.nombreaudio);
      text.setFont(font);
      text.setColor(sf::Color::Yellow);
      text.setOrigin(sf::Vector2f(text.getLocalBounds().width/2.0,text.getLocalBounds().height/2.0));
      text.setScale(sf::Vector2f(float(ladoboton)/text.getLocalBounds().height,float(ladoboton)/text.getLocalBounds().height));
      text.setPosition(sf::Vector2f(window.getSize().x/2.0,window.getSize().y-5*ladoboton));
      window.draw(text);
    }
  }
  sf::View view;
  view.setCenter(sf::Vector2f(window.getSize().x/2.0,window.getSize().y/2.0));
  view.setSize(sf::Vector2f(window.getSize().x,window.getSize().y));
  window.setView(view);
  



  /*
  sf::View view;
  view.setCenter(sf::Vector2f(e.xcamara,e.ycamara));
  view.setSize(sf::Vector2f(e.anchocamara,e.altocamara));
  window.setView(view);
  */
  window.display();
  //stretch_blit(portatil,pantalla,0,0,portatil->w,portatil->h,180,400,130,100);
  //stretch_blit(portatil,pantalla,0,0,portatil->w,portatil->h,650,400,130,100);
  //draw_sprite(pantalla,obtenerdibujo("portatil"),650,400);
  /*
  if (e.luz!=255) {
    for (int x=0;x<1000;x++) {
      for (int y=0;y<700;y++) {
	int color=getpixel(pantalla,x,y);
	putpixel(pantalla,x,y,makecol(getr(color)*e.luz/255,getg(color)*e.luz/255,getb(color)*e.luz/255));
      }
    }
  }
  string s="FRAME: "+itos(numframe);
  textout_ex(pantalla,font,s.c_str(),10,10,
	       makecol(255,0,0),makecol(0,0,0));
  */
}

sf::Clock reloj;

void iniciartiempo()
{
  reloj.restart();
}

void esperartiempo()
{
  sf::Time esperar=sf::seconds(1.0/framespersecond);
  bool hayespera=false;
  while (reloj.getElapsedTime()<esperar) hayespera=true;
  if (not hayespera) cout<<"No hay espera"<<endl;
  reloj.restart();
}

string obtenerfechamodificacioncomandos()
{
#ifdef __APPLE__
  system("ls -lT comandos.txt >listaficheros.txt");
#elif __linux
    system("ls --full-time comandos.txt >listaficheros.txt");
#endif
    return leelineafichero("listaficheros.txt");
}

void ejecutaremision()
{
  cargardibujos();
  cargargraficossoporte();
  cargaraudio();
  cargarfont();


  sf::Sound sound;


  vector<estadogeneral> ve;

  cargarve(ve);
  if (int(ve.size())<=1) {
    cout<<"Error: no hay frames"<<endl;
    exit(0);
  }
  string fechamodificacioncomandos=obtenerfechamodificacioncomandos();

  window.create(sf::VideoMode(anchowindow,altowindow),"Dibujos animados");
  
  iniciartiempo();

  int framestop=0;
  string accion="stop";
  if (not haybotones)
    accion="play";
  int faseaccion=0;
  int periodoaccion=2;
  int frame=0;

  for (;;) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
	window.close();
	exit(0);
	break;
      case sf::Event::KeyPressed:
	if (event.key.code == sf::Keyboard::Escape) {
	  window.close();
	  exit(0);
	}
	break;

      case sf::Event::MouseButtonPressed:
	if (event.mouseButton.button==sf::Mouse::Left) {
	  int x=event.mouseButton.x;
	  int y=event.mouseButton.y;
	  if (boton2rect["play"].contains(x,y)) {
	    accion="play";
	    sound.stop();
	    if (ve[frame].accionaudio==2) {
	      sf::SoundBuffer &soundbuffer=obteneraudiobuffer(ve[frame].nombreaudio);
	      sound.setBuffer(soundbuffer);
	      sound.setPlayingOffset(ve[frame].tantoaudio*soundbuffer.getDuration());
	      sound.setPitch(1.0);
	      sound.play();
	    }
	  } else if (boton2rect["slow"].contains(x,y) or boton2rect["slowstop"].contains(x,y)) {
	    accion=boton2rect["slow"].contains(x,y)?"slow":"slowstop";
	    faseaccion=0;
	    sound.stop();
	    if (ve[frame].accionaudio==2) {
	      sf::SoundBuffer &soundbuffer=obteneraudiobuffer(ve[frame].nombreaudio);
	      sound.setBuffer(soundbuffer);
	      sound.setPlayingOffset(ve[frame].tantoaudio*soundbuffer.getDuration());
	      sound.setPitch(1.0/periodoaccion);
	      sound.play();
	    }
	  } else if (boton2rect["slowrev"].contains(x,y)) {
	    accion="slowrev";
	    framestop=frame;
	    faseaccion=0;
	    sound.stop();
	  } else if (boton2rect["pause"].contains(x,y)) {
	    accion="stop";
	    framestop=frame;
	    sound.stop();
	  } else if (boton2rect["stop"].contains(x,y)) {
	    accion="stop";
	    frame=framestop;
	    sound.stop();
	  } else if (boton2rect["rev"].contains(x,y)) {
	    accion="stop";
	    framestop=0;
	    frame=0;
	    sound.stop();
	  } else if (boton2rect["barra"].contains(x,y)) {
	    frame=float(x-boton2rect["barra"].left)/boton2rect["barra"].width*(int(ve.size())-1);
	    if (frame>int(ve.size())-1) frame=int(ve.size())-1;
	    if (frame<0) frame=0;
	    framestop=frame;
	    sound.stop();
	    if (accion=="play" and ve[frame].accionaudio==2) {
	      sf::SoundBuffer &soundbuffer=obteneraudiobuffer(ve[frame].nombreaudio);
	      sound.setBuffer(soundbuffer);
	      sound.setPlayingOffset(ve[frame].tantoaudio*soundbuffer.getDuration());
	      sound.setPitch(1.0);
	      sound.play();
	    }
	  }
	}
	break;

      case sf::Event::MouseButtonReleased:
	if (accion=="slow" or accion=="slowrev") {
	  accion="stop";
	  framestop=frame;
	  sound.stop();
	} else if (accion=="slowstop") {
	  accion="stop";
	  frame=framestop;
	  sound.stop();
	}
	break;


      default:
	break;
      }
    }
    if (accion=="play") {
      if (ve[frame].accionaudio==1) {
	sound.stop();
	sound.setBuffer(obteneraudiobuffer(ve[frame].nombreaudio));
	sound.setPitch(1.0);
	sound.play();
      }
    } else if (accion=="slow" or accion=="slowstop") {
      if (faseaccion==0 and ve[frame].accionaudio==1) {
	sound.stop();
	sound.setBuffer(obteneraudiobuffer(ve[frame].nombreaudio));
	sound.setPitch(1.0/periodoaccion);
	sound.play();
      }
    }

    dibujarestadogeneral(ve[frame],frame,int(ve.size()));

    if (accion=="play") {
      frame++;
      if (frame==int(ve.size())) {
	accion="stop";
	frame=framestop;
	sound.stop();
      }
    } else if (accion=="slow" or accion=="slowstop") {
      faseaccion++;
      if (faseaccion==periodoaccion) {
	faseaccion=0;
	frame++;
	if (frame==int(ve.size())) {
	  accion="stop";
	  frame=framestop;
	  sound.stop();
	}
      }
    } else if (accion=="slowrev") {
      faseaccion++;
      if (faseaccion==periodoaccion) {
	faseaccion=0;
	frame--;
	framestop=frame;
	if (frame<0) {
	  accion="stop";
	  frame=framestop=0;
	  sound.stop();
	}
      }
    } else if (accion=="stop") {
      string nextfechamodificacioncomandos=obtenerfechamodificacioncomandos();
      if (fechamodificacioncomandos!=nextfechamodificacioncomandos) {
	window.clear(sf::Color::Black);
	sf::Text text;
	text.setString("LOADING comandos.txt");
	text.setFont(font);
	text.setColor(sf::Color::Yellow);
  	text.setOrigin(sf::Vector2f(text.getLocalBounds().width/2.0,text.getLocalBounds().height/2.0));
	text.setPosition(sf::Vector2f(window.getSize().x/2.0,window.getSize().y/2.0));
	window.draw(text);
	window.display();
	cargarve(ve);
	if (int(ve.size())<=1) {
	  cout<<"Error: no hay frames"<<endl;
	  exit(0);
	}
	fechamodificacioncomandos=nextfechamodificacioncomandos;
	if (frame>int(ve.size())-1) frame=int(ve.size())-1;
	if (frame<0) frame=0;
      }
    }
    esperartiempo();
  }
}

void ejecutargrabacion()
{
  grabando=true;
  system("rm BMPS/*");
  cargardibujos();
  cargaraudio();
  vector<estadogeneral> ve;
  cargarve(ve);
  if (int(ve.size())<=1) {
    cout<<"Error: no hay frames"<<endl;
    exit(0);
  }
  renderTexture.create(anchowindow,altowindow);
  for (int frame=0;frame<int(ve.size());frame++) {
    cout<<"FRAME "<<frame+1<<" de "<<int(ve.size())<<endl;
    texturarestadogeneral(ve[frame]);
    salvartexturageneral("BMPS/frame"+itos5(frame)+".jpg");
  }
  string comando="cd BMPS && ffmpeg -f image2 -framerate "+itos(framespersecond)+" -i frame%05d.jpg video.avi";
  system(comando.c_str());
}


int main(int argc,char *argv[])
{
  if (argc>=2) {
    string s=argv[1];
    if (esentero(s)) {
      haybotones=false;
      desplazamientoaudio=mystoi(s);
    }
  }
  //if (argc>=2) ejecutargrabacion();
  //else
  ejecutaremision();
}


