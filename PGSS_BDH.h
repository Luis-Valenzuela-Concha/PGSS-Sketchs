#ifndef PGSS_BDH_H
#define PGSS_BDH_H

#include <vector>

using namespace std;


class PGSS_BDH{
    private:
        vector<vector<vector<vector<unordered_map<int,int>>>>> sketch;
        int m; //Tamaño matriz
        int T; //Marca de tiempo maxima
        int k; //Funciones hash
        vector<vector<vector<int>>> insertados;
    public:
        PGSS_BDH(int m, int T, int k);
        ~PGSS_BDH();
        int query(int s, int d, int ts, int te);
        void update(int s, int d, int w, int t);
        int useHash(int n, int hash);
        void print(int x, int y, int hash);
        vector<pair<int,int>> find_anomalia(int s, int d, int ts, int te, float precision, int umbral);
};

#endif