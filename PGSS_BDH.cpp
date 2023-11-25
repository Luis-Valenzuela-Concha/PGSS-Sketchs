#include <bits/stdc++.h>
#include <iostream>
#include <unordered_map>

#include "PGSS_BDH.h"
#include "hashes/City.h"
#include "hashes/Hashes.h"
#include "hashes/MurmurHash2.h"
#include "hashes/MurmurHash3.h"

using namespace std;

PGSS_BDH::PGSS_BDH(int m, int T, int k){ //m = tamaño matriz, T = marca de tiempo maxima, k = cantidad de f.hash
    this->m = m; this->T = T; this-> k = k; 
    int L = log2(T) + 1;
    sketch = vector<vector<vector<vector<unordered_map<int,int>>>>>
             (k, vector<vector<vector<unordered_map<int,int>>>>
             (m, vector<vector<unordered_map<int,int>>>
             (m, vector<unordered_map<int,int>>
             (L, unordered_map<int,int>()))));
    insertados = vector<vector<vector<int>>> 
                 (k, vector<vector<int>> 
                 (m, vector<int>(m,0)));
    promedio = vector<vector<vector<int>>> 
                 (k, vector<vector<int>> 
                 (m, vector<int>(m,0)));
    desviacionAcumulada = vector<vector<vector<int>>> 
                 (k, vector<vector<int>> 
                 (m, vector<int>(m,0)));
    desviacion = vector<vector<vector<int>>> 
                 (k, vector<vector<int>> 
                 (m, vector<int>(m,0)));
}

PGSS_BDH::~PGSS_BDH(){;}

int PGSS_BDH::useHash(int n, int hash){
    int seed = 1;
    int size = this->m;
    uint32_t hash_value;
    switch (hash) {
        case 0:
            hash_value = MurmurHash64A(&n, sizeof(unsigned int), seed) % size;
            break;
        case 1:
            hash_value = CityHash64WithSeed((const char*)&n, sizeof(unsigned int), seed) % size;
            break;
        case 2:
            uint32_t hash;
            MurmurHash3_x86_32(&n, sizeof(unsigned int), seed, &hash);
            hash_value = hash % size;
            break;
        case 3:
            hash_value = Crap8((const uint8_t*)&n, sizeof(unsigned int), seed) % size;
            break;
        default:
            break;
    }
    return hash_value;
}

void PGSS_BDH::update(int s, int d, int w, int t){
    bool anomalia = true;
    int end = ceil(log2(T));
    //cout << t <<") "<<"Hashes:" << endl;
    for(int hash = 0; hash < k ; hash++){
        int x = useHash(s,hash); // source
        int y = useHash(d,hash); // destination 
        for(int l = 0; l <= end ; l++){
            int g = pow(2,l);
            int c = ceil(float(t)/float(g));
            if(c==0) c = 1;
            if(sketch[hash][x][y][l].find(c) == sketch[hash][x][y][l].end()){
                sketch[hash][x][y][l][c] = w; //si no existe lo inserta
            }else{
                sketch[hash][x][y][l].at(c) += w; //existe lo suma
            }
        }
        insertados[hash][x][y]++;
        /*promedio[hash][x][y] = sketch[hash][x][y][ceil(log2(T))].at(1) / insertados[hash][x][y];
        desviacionAcumulada[hash][x][y] += pow(w - promedio[hash][x][y],2);
        desviacion[hash][x][y] = sqrt(desviacionAcumulada[hash][x][y] / insertados[hash][x][y]);*/
        //cout << hash << " promedio: " << promedio[hash][x][y] << " desviacion: " << desviacion[hash][x][y] << endl;


    }
    /*for(int hash = 0; hash < k ; hash++){
        int x = useHash(s,hash); // source
        int y = useHash(d,hash); // destination 
        int inf = promedio[hash][x][y] - 1.5*float(desviacion[hash][x][y]);
        int sup = promedio[hash][x][y] + 1.5*float(desviacion[hash][x][y]);
        if(inf <= w && w <= sup){
            anomalia = false;
        }
        
    }
    if(anomalia){
        cout << t <<") "<<"Anomalia: " << s << " " << d << " " << w << endl;
    }*/
}

vector<pair<int,int>> GetPerfectCover(int ts, int te){
    vector<pair<int,int>> I;
    int t1 = ts, t2;
    while(t1 <= te){
        if(t1 % 2 == 0){
            t2 = t1;
        }else{
            int g = 2;
            while(t1 % (g*2) == 1){
                g = g * 2;
                if(t1 == 1) break; //No se atrapa en el bucle si ts = 1;
            }
            int g2 = pow(2, floor( float(log2(te-t1+1)) ));
            t2 = t1 + min(g,g2) - 1;
        }
        I.push_back(pair<int,int>(t1,t2));
        t1 = t2 + 1;
    }
    return I;
}

int PGSS_BDH::query(int s, int d, int ts, int te){
    pair <int,int> time_interval = pair<int,int>(ts,te);
    vector<pair<int,int>> I = GetPerfectCover(ts,te);
    int  W = 0;
    for(int i = 0; i < I.size(); i++){
        int l = log2(I[i].second - I[i].first + 1);
        int g = pow(2,l);
        int c = ceil(float(I[i].first) / float(g));
        int w = INT_MAX;
        for(int hash = 0 ; hash < k ; hash++){
            int x = useHash(s,hash); int y = useHash(d,hash);
            if(sketch[hash][x][y][l].find(c) != sketch[hash][x][y][l].end()){
                w = min(w, sketch[hash][x][y][l].at(c));
            }else{
                w = 0;
            }
        }
        W += w;
    }
    return W;
}

vector<pair<int,int>> PGSS_BDH:: find_anomalia(int s, int d, int ts, int te, float precision, int umbral){
    vector<pair<int,int>> anomalias;
    int rango_total = te-ts+1;
    int particiones = rango_total*precision;
    if(rango_total < particiones) particiones = rango_total;
    int rango_intervalo = rango_total/particiones;

    //Crea subintervalos
    vector<pair<int,int>> I;
    for(int i = 0; i < particiones ; i++){
        int ts1 = ts + rango_intervalo * i;
        int te1 = ts + rango_intervalo * (i+1) - 1;
        I.push_back(pair<int,int>(ts1,te1));
    }
    //Ajusta el ultimo intervalo
    if(I[I.size()-1].second < te){
        I.push_back(pair<int,int>(I[I.size()-1].second+1,te));
        particiones++;
    }

    //Calcula el flujo promedio
    int hash = 0;
    int x = useHash(s,hash); int y = useHash(d,hash);
    int promedio = sketch[hash][x][y][ceil(log2(T))].at(1) / insertados[hash][x][y];
    //cout << "Promedio: " << promedio << endl;
    //Calcula la anomalia
    for(int i = 0; i < I.size(); i++){
        int w = query(s,d,I[i].first,I[i].second);
        if(w == 0) continue;
        int promedio_intervalo = promedio * (I[i].second - I[i].first + 1);
        /*cout << "Intervalo: [" << I[i].first << "," << I[i].second << "]" << endl;
        cout << "Promedio intervalo: " << promedio_intervalo << endl;
        cout << "w: " << w << endl;*/
        int diferencia = abs(w - promedio_intervalo);
        if(diferencia > umbral){
            anomalias.push_back(I[i]);
        }
    }
    return anomalias;
}

void PGSS_BDH::print(int x,int y, int hash){
    int end = ceil(log2(T));
    cout << "Hash " << hash << endl;
    for(int l = 0; l <= end ; l++){
        cout << "l:" << l << " ";
        int g = pow(2,l);
        int c = ceil(float(T)/float(g));
        cout << "c:" << c << " ";
        for(int j = 1 ; j <= c ; j++){
            if(sketch[hash][x][y][l].find(j) != sketch[hash][x][y][l].end()){
                cout << "<" << j << ":" << sketch[hash][x][y][l].at(j) << "> ";
            }else{
                cout << "<" << j << ":" << 0 << "> ";
            }
        }
        cout << endl;
    }
}