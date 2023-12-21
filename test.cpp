#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "PGSS_BDH.h"
#include "hashes/City.h"
#include "hashes/Hashes.h"
#include "hashes/MurmurHash2.h"
#include "hashes/MurmurHash3.h"
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

vector<vector<string>> return_anomalias(vector<vector<string>> conexion){
    vector<vector<string>> anomalias;
    int promedio = 0;
    for(vector<string> linea : conexion){
        promedio += stoi(linea[6]);
    }
    promedio = promedio/conexion.size();
    int desviacion = 0;
    for(vector<string> linea : conexion){
        desviacion += pow(stoi(linea[6]) - promedio,2);
    }
    desviacion = sqrt(desviacion/(conexion.size()));

    for(vector<string> linea : conexion){
        if(stoi(linea[6]) < promedio - 1*desviacion || stoi(linea[6]) > promedio + 1*desviacion){
            anomalias.push_back(linea);
        }
    }
    return anomalias;
}

int main(){
    // Pruebas experimentales

    int time = 262144;
    vector<vector<string>> file = copiarArchivo("datasets/chicago2015_withdata.txt",time);
    
    //Separa las conexiones por IP1_IP2
    unordered_map<string, vector<vector<string>>> separated;
    for(vector<string> linea : file){
        string key = linea[4] + "_" + linea[5]; // Combina la primera y la segunda IP para crear la clave
        separated[key].push_back(linea);
    }
    vector<vector<vector<string>>> separados;
    vector<pair<string, vector<vector<string>>>> conexiones(separated.begin(), separated.end());
    for(auto it = separated.begin(); it != separated.end(); it++){
        vector<vector<string>> conexion;
        for(const auto& linea : it->second){
            conexion.push_back(linea);
        }
        separados.push_back(conexion);
    }

    //Calcula las anomalias reales
    vector<vector<string>> anomalias_reales;
    for(vector<vector<string>> conexion : separados){
        vector<vector<string>> anomalias_conexion = return_anomalias(conexion);
        for(vector<string> linea : anomalias_conexion){
            anomalias_reales.push_back(linea);
        }
    }

    //Almacena las anomalias reales en un set
    set<tuple<string,string,string,string>> anomalias_reales_set;
    for(vector<string> linea : anomalias_reales){
        anomalias_reales_set.insert(make_tuple(linea[0],linea[1],linea[2],linea[3]));
    }

    freopen("recall_precision.txt","w",stdout);
    for(int k = 2 ; k<=5 ; k++){
        int m = 1000; int T = time;
        PGSS_BDH sketch (m,T,k);

        int i = 1;
        for(vector<string> linea : file){
            sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
            i++;
        }
        //Calcula las anomalias estimadas
        vector<vector<string>> anomalias_estimadas;
        for (vector<vector<string>> conexion : separados){
            vector<pair<int,int>> anomalias = sketch.find_anomalia(stoi(conexion[0][4]),stoi(conexion[0][5]),1,time,1);
            for(pair<int,int> anomalia : anomalias){
                anomalias_estimadas.push_back(file[anomalia.first-1]);
            }
        }
        //Almacena las anomalias estimadas en un set
        set<tuple<string,string,string,string>> anomalias_estimadas_set;
        for(vector<string> linea : anomalias_estimadas){
            anomalias_estimadas_set.insert(make_tuple(linea[0],linea[1],linea[2],linea[3]));
        }

        //Calcula precision y recall
        int TP = 0, FP = 0, FN = 0;
        for(tuple<string,string,string,string> anomalia : anomalias_estimadas_set){
            if(anomalias_reales_set.find(anomalia) != anomalias_reales_set.end()){
                TP++;
            }else{
                FP++;
            }
        }
        for(tuple<string,string,string,string> anomalia : anomalias_reales_set){
            if(anomalias_estimadas_set.find(anomalia) == anomalias_estimadas_set.end()){
                FN++;
            }
        }
        double precision = (double)TP/(double)(TP+FP);
        double recall = (double)TP/(double)(TP+FN);
        cout << "Funciones hash: " << k << endl;
        cout << "Precision: " << precision << endl;
        cout << "Recall: " << recall << endl << endl;
    }

    //Calculo de espacio del sketch
    /*freopen("size.txt","w",stdout);
    for (int k=2;k<=5;k++){
        PGSS_BDH sketch (m,T,k);
        int i = 1;
        for(vector<string> linea : file){
            sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
            i++;
        }

        cout << "Funciones hash: " << k << endl;
        cout << "Calculo de tamaño" << endl;
        cout << "Tamaño de la estructura: " << sketch.size_in_bytes() << " bytes" << endl << endl;
    }*/

    //Calculo de tiempo de consulta
    /*int rep = 10;
    freopen("time.txt","w",stdout);
    for(int i = 100000; i < time ; i+=100000){
        auto tiempo_consulta = 0;
        for(int r = 0; r<rep; r++){
            auto start = chrono::high_resolution_clock::now();
            vector<pair<int,int>> anomalias = sketch.find_anomalia(stoi(file[0][4]),stoi(file[0][5]),1,i,2);
            auto finish = chrono::high_resolution_clock::now();
            tiempo_consulta += chrono::duration_cast<chrono::milliseconds> (finish - start).count();
        }
        tiempo_consulta = tiempo_consulta/rep;
        cout << "Tiempo de consulta para intervalo de " << i << " datos: " << tiempo_consulta << " ms" << endl << endl;
    }*/
    return 0;
}