#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "PGSS_BDH.h"
#include <chrono>

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
    int n = 262144;
    vector<vector<string>> file = copiarArchivo("datasets/chicago2015_withdata.txt",n);
    printf("Se extraen %d datos en el intervalo:\n",n);
    printf("[%s %s %s %s , %s %s %s %s]\n",file[0][0].c_str(),file[0][1].c_str(),file[0][2].c_str(),file[0][3].c_str(),
    file[n-1][0].c_str(),file[n-1][1].c_str(),file[n-1][2].c_str(),file[n-1][3].c_str());

    vector<int> IPs;
    for (vector<string> linea : file){
        IPs.push_back(stoi(linea[4]));
        IPs.push_back(stoi(linea[5]));
    }
    sort(IPs.begin(),IPs.end());
    IPs.erase(unique(IPs.begin(),IPs.end()),IPs.end());


    /* Inicializa sketch */
    int m = 1000; int T = n; int k = 3;
    PGSS_BDH sketch (m,T,k);

    printf("\nSe reduce tamaño de n = %ld a m = %d",IPs.size(),m);

    /* Inserta datos */
    cout << "\nInsertando...\n" << endl;
    int i = 1;
    for(vector<string> linea : file){
        sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
        i++;
    }

    /* Busca anomalias */
    vector<string> linea = file[0];
    int s = stoi(linea[4]); int d = stoi(linea[5]); int ts = 1; int te = n;
    printf("Buscando anomalias en conexion IPs: %d %d ...\n\n",s,d);

    float precision = 1;
    auto start = chrono::high_resolution_clock::now();
    vector<pair<int,int>> anomalias = sketch.find_anomalia(s,d,ts,te,precision);
    auto finish = chrono::high_resolution_clock::now();
    auto time = chrono::duration_cast<chrono::milliseconds> (finish - start).count();
    
    /* Imprime anomalias */
    printf("Anomalias: %ld\n",anomalias.size());
    for(pair<int,int> intervalo : anomalias){
        printf("[%s %s %s %s]\n",file[intervalo.first-1][0].c_str(),file[intervalo.first-1][1].c_str(),
        file[intervalo.first-1][2].c_str(),file[intervalo.first-1][3].c_str());
    }
    printf("\nTiempo de ejecucion: %ld milisegundos\n",time);

    return 0;
}