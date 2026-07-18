/* 
"dinámico" significa que el directorio de punteros se duplica solo cuando hace falta más espacio.
tenés un directorio (un array de punteros a "buckets"), y una función hash que te dice a qué bucket va cada usuario según su nombre. Cada bucket aguanta pocos usuarios (una capacidad chica). 
Cuando un bucket se llena, en vez de crecer indefinidamente, se divide en dos y si el directorio ya no tiene espacio para apuntar a ambos, el directorio 
también se duplica de tamaño
*/
#pragma once
#include "modelos.h"
#include <vector>
#include <string>
using namespace std;
struct Bucket {
    vector<Usuario> usuarios;
    int profundidadLocal;
    int capacidad;
    Bucket(int profLocal, int cap = 2);
};
class DynamicHash {
private:
    vector<Bucket*> directorio;
    int profundidadGlobal;
    int calcularHash(const string& clave);
    void dividirBucket(int indice);
public:
    DynamicHash();
    void insertar(const Usuario& user);
    Usuario* buscarPorNombre(const string& nombre);
    vector<Usuario> buscarPorCarrera(const string& carrera);
    vector<Usuario> buscarPorInstitucion(const string& institucion);
    vector<Usuario> obtenerTodos();
    void guardarEnArchivo(const string& nombreArchivo);
    void cargarDesdeArchivo(const string& nombreArchivo);
};