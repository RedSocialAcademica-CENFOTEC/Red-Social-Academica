#ifndef DYNAMICHASH_H
#define DYNAMICHASH_H

#include "modelos.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class DynamicHash {
private:
    struct Bucket {
        int profundidadLocal;
        vector<Usuario> datos;

        Bucket() : profundidadLocal(1) {}
    };

    int profundidadGlobal;
    vector<Bucket*> directorio;
    int capacidadBucket; // Número máximo de elementos por bucket

    // Funciones de hash
    size_t hashNombre(const string& nombre) const;
    size_t hashCarrera(const string& carrera) const;
    size_t hashInstitucion(const string& institucion) const;

    // Métodos privados
    void duplicarDirectorio();
    void dividirBucket(int indice);
    void reubicarBucket(int indice);
    size_t indiceHash(const string& clave, int prof) const;

public:
    DynamicHash(int _capacidadBucket = 4);
    ~DynamicHash();

    // Insertar y buscar
    void insertar(const Usuario& usuario);
    vector<Usuario> buscarPorNombre(const string& nombre) const;
    vector<Usuario> buscarPorCarrera(const string& carrera) const;
    vector<Usuario> buscarPorInstitucion(const string& institucion) const;
    vector<Usuario> buscar(const string& campo, const string& valor) const;

    // Persistencia
    bool guardar(const string& nombreArchivo) const;
    bool cargar(const string& nombreArchivo);

    void imprimir() const;
    int size() const;
    bool vacio() const;
};

#endif