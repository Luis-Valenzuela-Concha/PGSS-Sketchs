#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "PGSS_BDH.h"
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

int cantidadAnomalias(vector<vector<string>> conexion){
    int anomalias = 0;
    int promedio = 0;
    for(vector<string> linea : conexion){
        promedio += stoi(linea[2]);
    }
    promedio = promedio/conexion.size();
    int desviacion = 0;
    for(vector<string> linea : conexion){
        desviacion += pow(stoi(linea[2]) - promedio,2);
    }
    desviacion = sqrt(desviacion/(conexion.size()));

    for(vector<string> linea : conexion){
        if(stoi(linea[2]) < promedio - 1*desviacion || stoi(linea[2]) > promedio + 1*desviacion){
            anomalias++;
        }
    }
    return anomalias;
}

int main(){
    int time = 262144;
    vector<vector<string>> fileNoTimeStamp = copiarArchivoSinTimestamp("datasets/chicago2015_withdata.txt",time);
    vector<vector<string>> file = copiarArchivo("datasets/chicago2015_withdata.txt",time);

    unordered_map<string, vector<vector<string>>> separated;
    vector<vector<vector<string>>> conexiones;
    for (const auto& vec : fileNoTimeStamp) {
        separated[vec[1]].push_back(vec);
    }
    for (const auto& vec : separated) {
        vector<vector<string>> conexion;
        for (const auto& vec2 : vec.second) {
            conexion.push_back(vec2);
        }
        conexiones.push_back(conexion);
    }

    int m = 1000; int T = time; int k = 3;
    PGSS_BDH sketch (m,T,k);

    cout << "\nInsertando...\n" << endl;
    int i = 1;
    for(vector<string> linea : file){
        sketch.update(stoi(linea[4]),stoi(linea[5]),stoi(linea[6]),i);
        i++;
    }

    freopen("output.txt","w",stdout);
    float ERM;
    int valor_exacto = 0;
    int valor_estimado = 0;
    cout << "Cantidad de conexiones: " << conexiones.size() << endl;
    for (vector<vector<string>> conexion : conexiones){ 
        cout << "Conexion: " << conexion[0][0] << " - " << conexion[0][1] << endl;
        cout <<"Anomalias reales: "<< cantidadAnomalias(conexion) << endl;
        valor_exacto += cantidadAnomalias(conexion);
        vector<pair<int,int>> anomalias = sketch.find_anomalia(stoi(conexion[0][0]),stoi(conexion[0][1]),1,time,1);
        cout <<"Anomalias detectadas: "<< anomalias.size() << endl;
        valor_estimado += anomalias.size();
        cout << endl;
    }
    ERM = (float)abs(valor_exacto - valor_estimado)/(float)valor_exacto;
    cout << "ERM: " << ERM << endl;
    return 0;
}