#pragma once
#include "modelos.h"
#include <vector>
using namespace std;

//En cada nodo hoja, mantenés dos vectores en paralelo — uno de claves (fecha)

struct NodoB {
    bool esHoja;
    vector<int> claves;                  // fechas en formato AAAAMMDD
    vector<Publicacion> publicaciones;   // solo se usa si esHoja = true, paralelo a claves
    vector<NodoB*> hijos;                // solo se usa si esHoja = false
    NodoB* siguiente;                    // enlaza las hojas entre si (para recorrer el feed en orden) // siguiente es la clave del Árbol B+
    NodoB* padre;                        // para poder subir al dividir // una hoja se llena y hay que dividirla, un puntero al padre de cada nodo.
                                        // cuando una hoja se divide, hay que avisarle a su nodo padre que ahora tiene un hijo nuevo, y sin ese puntero no hay forma de "subir" en el árbol.

    NodoB(bool hoja);
};

class BPlusTree {
private:
    NodoB* raiz;
    int orden;   // cantidad maxima de claves por nodo antes de dividir // cada nodo aguanta hasta 4 claves.

    NodoB* buscarHoja(int clave);
    void dividirHoja(NodoB* hoja);
    void insertarEnPadre(NodoB* padre, int clave, NodoB* nuevoHijo);

public:
    BPlusTree(int _orden = 4);
    void insertar(const Publicacion& pub);
    vector<Publicacion> obtenerFeed(int fechaInicio, int fechaFin);
    bool eliminar(int idPublicacion);
    void imprimirHojas() const;

    vector<Publicacion> obtenerTodas();
    void guardarEnArchivo(const string& nombreArchivo);
    void cargarDesdeArchivo(const string& nombreArchivo);
};