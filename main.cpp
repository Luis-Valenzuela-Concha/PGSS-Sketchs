#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "PGSS_BDH.h"

using namespace std;

vector<vector<string>> copiarArchivo(string nombreArchivo, int n){
    ifstream archivo(nombreArchivo);
    vector<vector<string>> vec;
    string linea_archivo;
    while (getline(archivo, linea_archivo) and n>0) {
        vector<string> linea;
        linea.push_back(linea_archivo.substr(0, linea_archivo.find(" "))); //Fecha
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        linea.push_back(linea_archivo.substr(0, linea_archivo.find(" "))); //Hora
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        linea.push_back(linea_archivo.substr(0, linea_archivo.find(" "))); //Segundos
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        string nanosegundo = linea_archivo.substr(0, linea_archivo.find(" "));
        linea.push_back(nanosegundo.substr(0,nanosegundo.size()-3)); //NanoSegundos
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);

        linea.push_back(linea_archivo.substr(0, linea_archivo.find(" "))); //IP1
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        linea.push_back(linea_archivo.substr(0, linea_archivo.find(" "))); //IP 2
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);

        linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        linea_archivo.erase(0, linea_archivo.find(" ") + 1);

        linea.push_back(linea_archivo.substr(0, linea_archivo.find(" ")));

        vec.push_back(linea);
        n--;
    }
    archivo.close();
    return vec;
}

int main(){

    // Copia de archivo
    int n = 131072;
    vector<vector<string>> file = copiarArchivo("datasets/chicago2015_withdata.txt",n);
    cout << "Insertados:" << endl;
    cout << "[" << file[0][0] << " " << file[0][1] << " " << file[0][2] << " " << file[0][3] << " , ";
    cout << file[n-1][0] << " " << file[n-1][1] << " " << file[n-1][2] << " " << file[n-1][3] << "]" << endl;
    /*
    file[i][0] = fecha; file[i][1] = hora; file[i][2] = segundos; file[i][3] = nanosegundos; 
    file[i][4] = IP1; file[i][5] = IP2;
    file[i][6] = peso;
    */
    /*for(vector<string> linea : file){
        cout << linea[0] << " " << linea[1] << " " << linea[2] << " " << linea[3] << " " <<
        linea[4] << " " << linea[5] << " " << linea[6] << endl;
    }*/
    
    /*vector<unsigned int> ips;
    for(vector<string> linea : file){
        ips.push_back(stoi(linea[4]));
        ips.push_back(stoi(linea[5]));
    }
    sort(ips.begin(),ips.end());
    ips.erase(unique(ips.begin(),ips.end()),ips.end());
    cout << ips.size() << endl;*/

    int m = 800; int T = n; int k = 3;
    PGSS_BDH sketch (m,T,k);
    int i = 1;
    for(vector<string> linea : file){
        sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
        i++;
    }
    vector<string> linea = file[0];
    int s = stoi(linea[4]); int d = stoi(linea[5]); int ts = 1; int te = n;
    cout << s << " " << d << " " << stoi(linea[6]) << endl;

    float precision = 1; int umbral = 300;
    vector<pair<int,int>> anomalias = sketch.find_anomalia(s,d,ts,te, precision,umbral);
    
    cout << "Anomalias: " << anomalias.size() << endl;
    for(pair<int,int> intervalo : anomalias){
        cout << "[" << intervalo.first << "," << intervalo.second << "]" << endl;
        cout << "[" << file[intervalo.first-1][0] <<" "<<file[intervalo.first-1][1] << " " << 
        file[intervalo.first-1][2] << " " << file[intervalo.first-1][3] << " , ";

        cout << file[intervalo.second-1][0] <<" "<<file[intervalo.second-1][1] << " " << 
        file[intervalo.second-1][2] << " " << file[intervalo.second-1][3] << "]" << endl;;
    }

    return 0;
}