#pragma once
#include "modelos.h"
#include <vector>
#include <string>
using namespace std;

enum class Color { ROJO, NEGRO };

// Nodo del Árbol Rojo-Negro. Se indexa por id de Usuario.
// Se usa un nodo NIL centinela (en vez de nullptr) para simplificar la lógica
// de rotaciones y de rebalanceo al eliminar, tal como se vio en el taller de RB Tree:
// todas las "hojas" apuntan al mismo NIL, que siempre es NEGRO, así no hay que
// estar chequeando null en cada paso.
struct NodoRB {
    int id;             // clave = Usuario.id
    Usuario usuario;    // copia del usuario conectado
    Color color;
    NodoRB* izquierdo;
    NodoRB* derecho;
    NodoRB* padre;
    NodoRB(int _id, const Usuario& _usuario, NodoRB* nil);
};

class RedBlackTree {
private:
    NodoRB* NIL;   // centinela compartido por todo el árbol
    NodoRB* raiz;

    // Rotaciones (mantienen la propiedad de árbol binario de búsqueda)
    void rotarIzquierda(NodoRB* x);
    void rotarDerecha(NodoRB* x);

    // Rebalanceo tras insertar/eliminar (recoloreo + rotaciones)
    void arreglarInsercion(NodoRB* z);
    void arreglarEliminacion(NodoRB* x);

    // Auxiliares de eliminación
    void trasplantar(NodoRB* u, NodoRB* v);
    NodoRB* minimo(NodoRB* nodo) const;
    NodoRB* buscarNodo(int id) const;

    // Recorridos y liberación de memoria
    void inOrden(NodoRB* nodo, vector<Usuario>& resultado) const;
    void liberar(NodoRB* nodo);

public:
    RedBlackTree();
    ~RedBlackTree();

    // Se llama cuando un usuario inicia sesión.
    // Devuelve false si el usuario ya estaba en línea (no se duplica).
    bool conectar(const Usuario& usuario);

    // Se llama cuando un usuario cierra sesión.
    // Devuelve false si el usuario no estaba en línea.
    bool desconectar(int idUsuario);

    bool estaEnLinea(int idUsuario) const;

    // Recorrido in-order: usuarios en línea ordenados por id
    vector<Usuario> listarEnLinea() const;

    int cantidadEnLinea() const;

    void guardarEnArchivo(const string& nombreArchivo) const;
    void cargarDesdeArchivo(const string& nombreArchivo);
};
