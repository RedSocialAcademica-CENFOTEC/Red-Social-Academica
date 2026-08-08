#pragma once
#include "modelos.h"
#include <vector>
#include <string>
using namespace std;

// Arbol Rojo-Negro que lleva la lista de usuarios "en linea" (con sesion
// iniciada en este momento), ordenados por id. Es una estructura aparte del
// grafo: el grafo guarda TODOS los usuarios registrados, este arbol solo
// guarda a quienes estan conectados ahora mismo, y sirve para poder
// consultar "quien esta en linea" con busqueda/insercion en O(log n) en
// vez de recorrer una lista.
enum class ColorNodo { ROJO, NEGRO };

struct NodoRB {
    Usuario dato;
    ColorNodo color;
    NodoRB* izquierdo;
    NodoRB* derecho;
    NodoRB* padre;
    NodoRB(const Usuario& u, NodoRB* nil);
};

class ArbolRojoNegro {
private:
    NodoRB* nil;   // nodo centinela: representa todas las "hojas" (NULL)
    NodoRB* raiz;

    void rotarIzquierda(NodoRB* x);
    void rotarDerecha(NodoRB* x);
    void insertarFixup(NodoRB* z);
    void eliminarFixup(NodoRB* x);
    void transplantar(NodoRB* u, NodoRB* v);
    NodoRB* buscarNodo(int id) const;
    NodoRB* minimo(NodoRB* x) const;
    void recorridoInorden(NodoRB* x, vector<Usuario>& resultado) const;
    void liberar(NodoRB* x);

public:
    ArbolRojoNegro();
    ~ArbolRojoNegro();

    void insertar(const Usuario& usuario);   // si el id ya estaba, actualiza el dato
    bool eliminar(int id);
    bool estaEnLinea(int id) const;
    Usuario obtener(int id) const;            // asume que existe; usar estaEnLinea antes
    int cantidadEnLinea() const;
    vector<Usuario> obtenerTodosEnLinea() const; // ordenados por id (recorrido inorden)
};
