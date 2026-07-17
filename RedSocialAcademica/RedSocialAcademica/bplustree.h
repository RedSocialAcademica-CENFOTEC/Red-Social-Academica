#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "modelos.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <iostream>

using namespace std;

template <typename T>
class BPlusTree {
private:
    struct Nodo {
        bool esHoja;
        vector<T> claves;
        vector<shared_ptr<Nodo>> hijos;
        shared_ptr<Nodo> siguiente; // Para hojas
        shared_ptr<Nodo> anterior;  // Para hojas

        Nodo(bool hoja = true) : esHoja(hoja), siguiente(nullptr), anterior(nullptr) {}
    };

    int grado;
    shared_ptr<Nodo> raiz;
    int cantidadElementos;

    // Métodos privados
    void insertarEnNodo(shared_ptr<Nodo>& nodo, const T& clave);
    void dividirNodo(shared_ptr<Nodo>& nodo, shared_ptr<Nodo>& padre, int indice);
    void insertarEnHoja(shared_ptr<Nodo>& hoja, const T& clave);
    shared_ptr<Nodo> buscarHoja(const T& clave) const;
    void recorrerHojas(shared_ptr<Nodo> nodo, vector<T>& resultado) const;
    void recorrerRango(shared_ptr<Nodo> nodo, const T& inicio, const T& fin, vector<T>& resultado) const;
    void serializarNodo(shared_ptr<Nodo> nodo, ofstream& archivo) const;
    shared_ptr<Nodo> deserializarNodo(ifstream& archivo);

public:
    BPlusTree(int _grado = 4);
    ~BPlusTree();

    void insertar(const T& clave);
    bool buscar(const T& clave) const;
    vector<T> buscarRango(const T& inicio, const T& fin) const;
    vector<T> obtenerTodos() const;
    bool eliminar(const T& clave);
    int size() const { return cantidadElementos; }
    bool vacio() const { return cantidadElementos == 0; }

    // Persistencia
    bool guardar(const string& nombreArchivo) const;
    bool cargar(const string& nombreArchivo);

    void imprimir() const;
};

// Implementación en el mismo archivo por ser template
#include "bplustree.cpp" // Necesario por templates

#endif