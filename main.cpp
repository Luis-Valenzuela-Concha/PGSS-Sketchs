#include <bits/stdc++.h>
#include <iostream>
#include "PGSS_BDH_Sketch.h"

using namespace std;

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
            int g2 = pow(2, floor( log2(te-t1+1) ));
            t2 = t1 + min(g,g2) - 1;
        }
        I.push_back(pair<int,int>(t1,t2));
        t1 = t2 + 1;
    }
    return I;
}

int main(){
    int ts,te;
    /*cin >> ts; cin >> te;
    vector<pair<int,int>> I = GetPerfectCover(ts,te);
    for(int i=0;i<I.size();i++){
        printf("[%d,%d]\n",I[i].first,I[i].second);
    }*/
    PGSS_BDH_Sketch sketch (3,16,3);
    sketch.update(1,2,4,4);
    sketch.update(1,2,10,10);
    return 0;
}