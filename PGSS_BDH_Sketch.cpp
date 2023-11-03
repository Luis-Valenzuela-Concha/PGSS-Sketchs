#include <bits/stdc++.h>
#include <iostream>
#include <unordered_map>

#include "PGSS_BDH_Sketch.h"

using namespace std;

PGSS_BDH_Sketch::PGSS_BDH_Sketch(int m, int T, int h){ //m = tamaño matriz, T = marca de tiempo maxima, h = cantidad de f.hash
    this->m = m; this->T = T;
    int L = log2(T) + 1;
    sketch = vector<vector<vector<unordered_map<int,int>>>> 
             (m,vector<vector<unordered_map<int,int>>>(m,vector<unordered_map<int,int>>(L)));
}

PGSS_BDH_Sketch::~PGSS_BDH_Sketch(){;}

int PGSS_BDH_Sketch::useHash(int n){
    return n;
}

void PGSS_BDH_Sketch::update(int s, int d, int w, int t){
    int x = useHash(s);
    int y = useHash(d);
    int end = ceil(log2(T));
    for(int l = 0; l < end ; l++){
        int g = pow(2,l);
        int c = ceil(t/g);
        if(sketch[x][y][l].find(c)==sketch[x][y][l].end()){
            sketch[x][y][l][c] = w; //si no existe lo inserta
        }else{
            sketch[x][y][l].at(c) += w; //existe lo suma
        }
    }
}
