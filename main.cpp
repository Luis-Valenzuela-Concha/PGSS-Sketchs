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
    int n = 32768;
    //int n = 128;
    //int n = 32;
    vector<vector<string>> file = copiarArchivo("datasets/chicago2015_withdata.txt",n);

    /* Rango de tiempo de aristas insertadas */
    cout << "Rango de insertados:" << endl;
    cout << "[" << file[0][0] << " " << file[0][1] << " " << file[0][2] << " " << file[0][3] << " , ";
    cout << file[n-1][0] << " " << file[n-1][1] << " " << file[n-1][2] << " " << file[n-1][3] << "]" << endl;

    /* Inicializar sketch */
    int m = 1000; int T = n; int k = 3;
    PGSS_BDH sketch (m,T,k);

    /* Insertar aristas */
    int i = 1;
    int inicio = i;
    int final;
    for(vector<string> linea : file){
        sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
        i++;
        if(i%100 == 0 || i == n){
            final = i;
            /*for(int j = inicio-1; j < final; j++){
                vector<string> linea = file[j];
                int s = stoi(linea[4]); 
                int d = stoi(linea[5]); 
                int ts = inicio; 
                int te = final;
                cout << "Intervalo: [" << ts << "," << te << "]" << endl;
                vector<pair<int,int>> anomalias = sketch.find_anomalia(s,d,ts,te, 1,300);
            }*/
            cout << "Intervalo: [" << inicio << "," << final << "]" << endl;
            inicio = final+1;
        }
    }
    /*vector<string> linea = file[0];
    int s = stoi(linea[4]); int d = stoi(linea[5]); int ts = 1; int te = n;
    cout << s << " " << d << " " << stoi(linea[6]) << endl;*/

    /* Obtener anomalias */
    /*float precision = 1; int umbral = 300;
    vector<pair<int,int>> anomalias = sketch.find_anomalia(s,d,ts,te, precision,umbral);*/
    
    /* Imprimir anomalias */
    /*cout << "Anomalias: " << anomalias.size() << endl;
    for(pair<int,int> intervalo : anomalias){
        cout << "[" << intervalo.first << "," << intervalo.second << "]" << endl;
        cout << "[" << file[intervalo.first-1][0] <<" "<<file[intervalo.first-1][1] << " " << 
        file[intervalo.first-1][2] << " " << file[intervalo.first-1][3] << " , ";

        cout << file[intervalo.second-1][0] <<" "<<file[intervalo.second-1][1] << " " << 
        file[intervalo.second-1][2] << " " << file[intervalo.second-1][3] << "]" << endl;;
    }*/

    return 0;
}