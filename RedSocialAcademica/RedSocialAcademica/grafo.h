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
    unordered_map<string, unordered_set<string>> adyacencia; // idUsuario -> set de ids amigos
    unordered_map<string, Usuario> usuarios; // idUsuario -> Usuario

    // BFS para encontrar camino más corto
    vector<string> bfs(const string& inicio, const string& destino) const;

public:
    Grafo();
    ~Grafo();

    // Gestión de usuarios
    void agregarUsuario(const Usuario& usuario);
    void eliminarUsuario(const string& id);
    bool existeUsuario(const string& id) const;
    Usuario obtenerUsuario(const string& id) const;

    // Gestión de amistades
    bool agregarAmistad(const string& id1, const string& id2);
    bool eliminarAmistad(const string& id1, const string& id2);
    bool sonAmigos(const string& id1, const string& id2) const;
    vector<string> obtenerAmigos(const string& id) const;

    // Sugerencias de amistad
    vector<string> sugerirAmigos(const string& id, int maxSugerencias = 5) const;
    vector<string> amigosEnComun(const string& id1, const string& id2) const;

    // Análisis de red
    int gradoDelNodo(const string& id) const;
    int distanciaEntre(const string& id1, const string& id2) const;
    vector<string> caminoMasCorto(const string& inicio, const string& destino) const;
    vector<vector<string>> componentesConexas() const;

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