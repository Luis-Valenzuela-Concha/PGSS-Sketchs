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

vector<vector<string>> copiarArchivoSinTimestamp(string nombreArchivo, int n){
    ifstream archivo(nombreArchivo);
    vector<vector<string>> vec;
    string linea_archivo;
    while (getline(archivo, linea_archivo) and n>0) {
        vector<string> linea;
        for(int i=0;i<4;i++){
            linea_archivo.erase(0, linea_archivo.find(" ") + 1);
        }
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
    int time = 262144/2/2;
    vector<vector<string>> file = copiarArchivo("datasets/chicago2015_withdata.txt",time);
    
    unordered_map<string, vector<vector<string>>> separated;
    for(vector<string> linea : file){
        string key = linea[4] + "_" + linea[5]; // Combina la primera y la segunda IP para crear la clave
        separated[key].push_back(linea);
    }
    //freopen("conexiones.txt","w",stdout);
    vector<vector<vector<string>>> separados;
    vector<pair<string, vector<vector<string>>>> conexiones(separated.begin(), separated.end());
    for(auto it = separated.begin(); it != separated.end(); it++){
        vector<vector<string>> conexion;
        for(const auto& linea : it->second){
            conexion.push_back(linea);
        }
        separados.push_back(conexion);
    }

    vector<vector<string>> anomalias_reales;
    for(vector<vector<string>> conexion : separados){
        vector<vector<string>> anomalias_conexion = return_anomalias(conexion);
        for(vector<string> linea : anomalias_conexion){
            anomalias_reales.push_back(linea);
        }
    }
    /*cout << "Anomalias reales:" << endl;
    for(vector<string> linea : anomalias_reales){
        cout << linea[0] << " " << linea[1] << " " << linea[2] << " " << linea[3] << " " << linea[4] << " " << linea[5] << " " << linea[6] << endl;
    }*/


    int m = 1000; int T = time; int k = 3;
    PGSS_BDH sketch (m,T,k);

    int i = 1;
    for(vector<string> linea : file){
        sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
        i++;
    }
    vector<vector<string>> anomalias_estimadas;
    for (vector<vector<string>> conexion : separados){
        vector<pair<int,int>> anomalias = sketch.find_anomalia(stoi(conexion[0][4]),stoi(conexion[0][5]),1,time,1);
        for(pair<int,int> anomalia : anomalias){
            anomalias_estimadas.push_back(file[anomalia.first-1]);
        }
    }
    /*cout << endl <<"Anomalias estimadas: " << endl;
    for(vector<string> linea : anomalias_estimadas){
        cout << linea[0] << " " << linea[1] << " " << linea[2] << " " << linea[3] << " " << linea[4] << " " << linea[5] << " " << linea[6] << endl;
    }*/

    int cantidad_anomalias_reales = anomalias_reales.size();
    int cantidad_anomalias_estimadas = anomalias_estimadas.size();
    cout << endl << "Cantidad de anomalias reales: " << cantidad_anomalias_reales << endl;
    cout << "Cantidad de anomalias estimadas: " << cantidad_anomalias_estimadas << endl;


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

    /*int rep = 10;
    freopen("size2.txt","w",stdout);
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

    /*freopen("test.txt","w",stdout);
    cout << "Calculo de ERM" << endl;
    cout << "Cantidad de conexiones: " << conexiones.size() << endl << endl;
    int m = 1000; int T = time;
    for(int k = 2; k <= 5 ; k++){
        cout << "k: " << k << endl;
        PGSS_BDH sketch (m,T,k);
        int i = 1;
        for(vector<string> linea : file){
            sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
            i++;
        }
  
        double ER;
        double suma_precision = 0;
        for (vector<vector<string>> conexion : conexiones){ 
            double valor_exacto = (double)cantidadAnomalias(conexion);
            vector<pair<int,int>> anomalias = sketch.find_anomalia(stoi(conexion[0][0]),stoi(conexion[0][1]),1,time,1);
            double valor_estimado = double(anomalias.size());
            if(valor_exacto == 0){
                valor_exacto += 1;
                valor_estimado += 1;
            }
            if(valor_estimado == valor_exacto) {
                suma_precision++;
            }
            double error = abs(valor_exacto - valor_estimado)/valor_exacto;
            ER += error;
        }

        double ERM = ER/conexiones.size();
        double precision = suma_precision/conexiones.size();
        cout << "ERM: " << ERM << endl;
        cout << "Precision: " << precision << endl << endl;
    }*/

    return 0;
}