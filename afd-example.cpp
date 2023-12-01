// AFD in C++ cu metode de setare/consultare/recunoastere a unui cuvant si
//  functii de citire si afisare:

/**************************************************************************/
#include <iostream>

#define getbit(s,i) ((s)[(i)/8] & 0x01<<(i)%8)
#define setbit(s,i) ((s)[(i)/8] |= 0x01<<(i)%8)
#define killbit(s,i) ((s)[(i)/8] &= ~(0x01<<(i)%8))
// testeaza, seteaza, respectiv anuleaza bitul cu nr. i>=0
//   numarand de la adresa de memorie s

#define MAX_NR_STARI_AFD 64

class AFD{
private:
    // nr. de stari; starea initiala este 0; -1=stare nedefinita
    int m_nrStari;

    // vector caracteristic al starilor finale;
    // bitul s%8 din octetul finala[s/8] este 1 sau 0
    //   dupa cum s este stare finala sau nu;
    unsigned char *m_finala;
    
    // tranzitiile;
    // daca din (s,c) se trece in d, atunci t[s][c]=d;
    // daca tranzitia din (s,c) nu este definita, t[s][c]=-1;
    int (*m_tranzitie)[256];

public:
    AFD();

    ~AFD();
    
    // ret. nr. de stari nrstari;
    int getnrstari();
    
    // seteaza nr. de stari cu pns si aloca memorie pt. automat;
    // ret. 1=succes, 0=esec;
    int setnrstari(int pns);

    // ret. ceva !=0 daca s este stare finala si 0 daca nu este stare finala;
    int estefinala(int s);

    // marcheaza starea s ca fiind finala;
    // ret. 1=succes, 0=esec;
    int setfinala(int s);

    // ret. starea in care se trece cu tranzitia din (s,c);
    // daca tranzitia din (s,c) nu este definita, ret. -1;
    int gettranzitie(int s, unsigned c);

    // incarca in automat tranzitia (s,c) -> d;
    // ret. 1=succes, 0=esec;
    int settranzitie(int s, unsigned c, int d);

    // ret. ceva !=0 daca AFD recunoaste cuvantul cuv
    //   si 0 daca nu-l recunoaste (inclusiv daca se blocheaza);
    // in *f furnizeaza starea in care AFD s-a oprit din procesarea lui cuv
    //   (daca nu l-a procesat pana la capat, e starea in care s-a blocat);
    int recunoaste(char const *cuv, int *f);
};

AFD::AFD()
{
    m_nrStari=0;
    m_finala=NULL; 
    m_tranzitie=NULL;
}

AFD::~AFD()
{
    delete m_finala;
    delete m_tranzitie;
}

int AFD::getnrstari()
{
    return m_nrStari;
}

int AFD::setnrstari(int pns){
    int i; 
    unsigned c;

    if(pns < 0 || pns > MAX_NR_STARI_AFD)
    {
        return 0;
    }
    m_nrStari = pns;
    
    delete m_finala;
    delete m_tranzitie;
    
    i = sizeof(unsigned char) * (7 + m_nrStari) / 8;

    if((m_finala = new unsigned char[i]) == NULL)
    {
        return 0;
    }

    while(i--)
    {
        m_finala[i]=0x00;
    }

    if((m_tranzitie=new int[m_nrStari][256]) == NULL)
    {
        delete m_finala;
        return 0;
    }

    for(i = 0; i < m_nrStari; ++i)
    {
        for(c = 0; c < 256; ++c)
        {
            m_tranzitie[i][c] = -1;
        }
    }
        
    return 1;
  }

int AFD::estefinala(int s)
{
    return getbit(m_finala,s)!=0;
}

int AFD::setfinala(int s){
   if(s<0 || s>=m_nrStari)return 0;
   setbit(m_finala,s);
   return 1;
 }

int AFD::gettranzitie(int s, unsigned c){return m_tranzitie[s][c];}

int AFD::settranzitie(int s, unsigned c, int d){
   if(s<0 || s>=m_nrStari || d<-1 || d>=m_nrStari)return 0;
   m_tranzitie[s][c]=d;
   return 1;
 }

int AFD::recunoaste(char const *cuv, int *f){
  int s=0;
  while(s!=-1 && *cuv!='\0'){
    s=m_tranzitie[s][*cuv];
    ++cuv;
  }
  if(s==-1)return 0;
  if(f)*f=s;
  return estefinala(s);
 }

int citeste(AFD &a){  // citire interactiva; ret. 1=succes,0=esec
    int ns, s, d;
    unsigned c;

    std::cout<<"Alfabet: setul ASCII axtins\n";
    std::cout<<"Dati numarul de stari: "; std::cin>>ns; if(!a.setnrstari(ns))return 0;
    std::cout<<"Starea initiala: 0\n";
    std::cout<<"Dati starile finale (-1 dupa ultima): ";
    
    std::cin>>s;
    while(s!=-1)
    {
        if(!a.setfinala(s))
        {
            return 0;
        }

        std::cin>>s;
    }
    
    std::cout<<"Dati tranzitiile (st_sursa, cod_zec_caracter, st_destinatie):\n";
    std::cout<<"  (dupa ultima tranzitie introduceti -1)\n";

    do
    {
        std::cin >> s; 
        if(s == -1)
        {
            break;
        }

        std::cin >> c >> d;
        if(!a.settranzitie(s,c,d))
        {
            return 0;
        }

    } while(true);

    return 1;
}
/* Exemplu de input:
4
2 -1
0 97 1
0 98 2
1 97 0
1 98 2
2 98 3
3 97 3
3 98 3
-1
*/

void afisaza(AFD &a){  // afisare;
    int ns;
    int i; 
    unsigned int c;
    
    ns=a.getnrstari();
    
    std::cout<<"Alfabet: setul ASCII axtins\n";
    std::cout<<"Numarul de stari: "<<ns<<"\n";
    std::cout<<"Starea initiala: 0\n";
    std::cout<<"Stari finale:";

    for(i=0; i<ns; ++i)
    {
        if(a.estefinala(i))
        {
            std::cout<<" "<<i;
        }
    }

    std::cout << "\n";
    std::cout << "Tranzitiile:\n";
    
    for(i = 0; i < ns; ++i)
    {
        for(c=0; c<32; ++c)
        {
            if(a.gettranzitie(i,c) != -1)
            {
                std::cout<<" delta("<<i<<",#"<<c<<")="<<a.gettranzitie(i,c)<<"\n";
            }
        }

        for(c = 32; c < 128; ++c)
        {
            if(a.gettranzitie(i,c) != -1)
            {
                std::cout<<" delta("<<i<<","<<(char)c<<")="<<a.gettranzitie(i,c)<<"\n";
            }
        }

        for(c = 128; c < 256; ++c)
        {
            if(a.gettranzitie(i,c) != -1)
            {
                std::cout<<" delta("<<i<<",#"<<c<<")="<<a.gettranzitie(i,c)<<"\n";
            }
        }
    }
}