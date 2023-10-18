#ifndef PGSS_BDH_SKETCH_H
#define PGSS_BDH_SKETCH_H

#include <vector>

using namespace std;

class PGSS_BDH_Sketch{
    private:
        vector<vector<vector<unordered_map<int,int>>>> sketch;
        int m;
        int T;
    public:
        PGSS_BDH_Sketch(int m, int T, int h);
        ~PGSS_BDH_Sketch();
        void update(int s, int d, int w, int t);
        int useHash(int n);
};

#endif