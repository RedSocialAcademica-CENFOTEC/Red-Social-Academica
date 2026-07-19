#ifndef GRAFO_H
#define GRAFO_H

#include "modelos.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <iostream>

using namespace std;

class Grafo {
private:
    unordered_map<int, unordered_set<int>> adyacencia; // idUsuario -> set de ids amigos
    unordered_map<int, Usuario> usuarios; // idUsuario -> Usuario

    // BFS para encontrar camino más corto
    vector<int> bfs(int inicio, int destino) const;

public:
    Grafo();
    ~Grafo();

    // Gestión de usuarios
    void agregarUsuario(const Usuario& usuario);
    void eliminarUsuario(int id);
    bool existeUsuario(int id) const;
    Usuario obtenerUsuario(int id) const;
    bool actualizarUsuario(int id, const Usuario& datosActualizados);
    vector<Usuario> obtenerTodosUsuarios() const;

    // Gestión de amistades
    bool agregarAmistad(int id1, int id2);
    bool eliminarAmistad(int id1, int id2);
    bool sonAmigos(int id1, int id2) const;
    vector<int> obtenerAmigos(int id) const;

    // Sugerencias de amistad
    vector<int> sugerirAmigos(int id, int maxSugerencias = 5) const;
    vector<int> amigosEnComun(int id1, int id2) const;

    // Análisis de red
    int gradoDelNodo(int id) const;
    int distanciaEntre(int id1, int id2) const;
    vector<int> caminoMasCorto(int inicio, int destino) const;
    vector<vector<int>> componentesConexas() const;

    // Estadísticas
    int cantidadUsuarios() const { return usuarios.size(); }
    int cantidadAmistades() const;
    double densidad() const;

    // Persistencia
    bool guardar(const string& nombreArchivo) const;
    bool cargar(const string& nombreArchivo);

    void imprimir() const;
};

#endif