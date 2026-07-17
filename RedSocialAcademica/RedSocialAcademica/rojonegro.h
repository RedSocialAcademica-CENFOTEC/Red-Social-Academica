#ifndef ROJONEGRO_H
#define ROJONEGRO_H

#include "modelos.h"
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

// Árbol Rojo-Negro para índices secundarios o caché
template <typename K, typename V>
class RedBlackTree {
private:
    enum Color { ROJO, NEGRO };

    struct Nodo {
        K clave;
        V valor;
        Color color;
        shared_ptr<Nodo> izquierdo, derecho, padre;

        Nodo(const K& _clave, const V& _valor)
            : clave(_clave), valor(_valor), color(ROJO),
            izquierdo(nullptr), derecho(nullptr), padre(nullptr) {
        }
    };

    shared_ptr<Nodo> raiz;
    int cantidadElementos;

    // Rotaciones
    void rotarIzquierda(shared_ptr<Nodo>& nodo);
    void rotarDerecha(shared_ptr<Nodo>& nodo);
    void balancearInsercion(shared_ptr<Nodo>& nodo);
    void balancearEliminacion(shared_ptr<Nodo>& nodo, bool ladoIzquierdo);

    // Operaciones auxiliares
    shared_ptr<Nodo> minimo(shared_ptr<Nodo> nodo) const;
    shared_ptr<Nodo> maximo(shared_ptr<Nodo> nodo) const;
    shared_ptr<Nodo> buscar(shared_ptr<Nodo> nodo, const K& clave) const;
    void insertar(shared_ptr<Nodo>& nodo, shared_ptr<Nodo>& padre, const K& clave, const V& valor);
    void eliminar(shared_ptr<Nodo>& nodo, const K& clave);
    void recorridoInorden(shared_ptr<Nodo> nodo, vector<pair<K, V>>& resultado) const;
    void serializar(shared_ptr<Nodo> nodo, ofstream& archivo) const;
    shared_ptr<Nodo> deserializar(ifstream& archivo, shared_ptr<Nodo> padre);

public:
    RedBlackTree();
    ~RedBlackTree();

    void insertar(const K& clave, const V& valor);
    bool buscar(const K& clave, V& valor) const;
    bool eliminar(const K& clave);
    vector<pair<K, V>> obtenerTodos() const;
    int size() const { return cantidadElementos; }
    bool vacio() const { return cantidadElementos == 0; }

    // Persistencia
    bool guardar(const string& nombreArchivo) const;
    bool cargar(const string& nombreArchivo);

    void imprimir() const;
};

// Incluir implementación (por ser template)
#include "rojonegro.cpp"

#endif