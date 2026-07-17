#include "grafo.h"
#include <algorithm>
#include <sstream>

Grafo::Grafo() {}

Grafo::~Grafo() {}

void Grafo::agregarUsuario(const Usuario& usuario) {
    if (!existeUsuario(usuario.id)) {
        usuarios[usuario.id] = usuario;
        adyacencia[usuario.id] = unordered_set<string>();
    }
}

void Grafo::eliminarUsuario(const string& id) {
    if (!existeUsuario(id)) return;

    // Eliminar todas las aristas que conectan con este usuario
    for (const auto& amigo : adyacencia[id]) {
        adyacencia[amigo].erase(id);
    }

    adyacencia.erase(id);
    usuarios.erase(id);
}

bool Grafo::existeUsuario(const string& id) const {
    return usuarios.find(id) != usuarios.end();
}

Usuario Grafo::obtenerUsuario(const string& id) const {
    auto it = usuarios.find(id);
    if (it != usuarios.end()) {
        return it->second;
    }
    throw runtime_error("Usuario no encontrado: " + id);
}

bool Grafo::agregarAmistad(const string& id1, const string& id2) {
    if (!existeUsuario(id1) || !existeUsuario(id2)) return false;
    if (id1 == id2) return false;
    if (sonAmigos(id1, id2)) return false;

    adyacencia[id1].insert(id2);
    adyacencia[id2].insert(id1);
    return true;
}

bool Grafo::eliminarAmistad(const string& id1, const string& id2) {
    if (!sonAmigos(id1, id2)) return false;

    adyacencia[id1].erase(id2);
    adyacencia[id2].erase(id1);
    return true;
}

bool Grafo::sonAmigos(const string& id1, const string& id2) const {
    auto it = adyacencia.find(id1);
    if (it == adyacencia.end()) return false;
    return it->second.find(id2) != it->second.end();
}

vector<string> Grafo::obtenerAmigos(const string& id) const {
    vector<string> resultado;
    auto it = adyacencia.find(id);
    if (it != adyacencia.end()) {
        for (const auto& amigo : it->second) {
            resultado.push_back(amigo);
        }
    }
    return resultado;
}

vector<string> Grafo::sugerirAmigos(const string& id, int maxSugerencias) const {
    vector<string> sugerencias;
    if (!existeUsuario(id)) return sugerencias;

    // Usar BFS para encontrar amigos de amigos
    unordered_set<string> visitados;
    queue<string> cola;
    unordered_map<string, int> distancia;

    // Inicializar con los amigos directos
    for (const auto& amigo : adyacencia.at(id)) {
        visitados.insert(amigo);
        cola.push(amigo);
        distancia[amigo] = 1;
    }
    visitados.insert(id);

    while (!cola.empty() && sugerencias.size() < maxSugerencias) {
        string actual = cola.front();
        cola.pop();

        for (const auto& vecino : adyacencia.at(actual)) {
            if (visitados.find(vecino) == visitados.end()) {
                visitados.insert(vecino);
                distancia[vecino] = distancia[actual] + 1;

                // Sugerir amigos a distancia 2 (amigos de amigos)
                if (distancia[vecino] == 2) {
                    sugerencias.push_back(vecino);
                    if (sugerencias.size() >= maxSugerencias) break;
                }

                cola.push(vecino);
            }
        }
    }

    return sugerencias;
}

vector<string> Grafo::amigosEnComun(const string& id1, const string& id2) const {
    vector<string> comunes;
    if (!existeUsuario(id1) || !existeUsuario(id2)) return comunes;

    const auto& amigos1 = adyacencia.at(id1);
    const auto& amigos2 = adyacencia.at(id2);

    for (const auto& amigo : amigos1) {
        if (amigos2.find(amigo) != amigos2.end()) {
            comunes.push_back(amigo);
        }
    }

    return comunes;
}

int Grafo::gradoDelNodo(const string& id) const {
    auto it = adyacencia.find(id);
    if (it == adyacencia.end()) return 0;
    return it->second.size();
}

vector<string> Grafo::caminoMasCorto(const string& inicio, const string& destino) const {
    if (inicio == destino) return { inicio };
    if (!existeUsuario(inicio) || !existeUsuario(destino)) return {};

    unordered_map<string, string> padre;
    unordered_set<string> visitados;
    queue<string> cola;

    cola.push(inicio);
    visitados.insert(inicio);

    while (!cola.empty()) {
        string actual = cola.front();
        cola.pop();

        if (actual == destino) break;

        for (const auto& vecino : adyacencia.at(actual)) {
            if (visitados.find(vecino) == visitados.end()) {
                visitados.insert(vecino);
                padre[vecino] = actual;
                cola.push(vecino);
            }
        }
    }

    if (visitados.find(destino) == visitados.end()) {
        return {}; // No hay camino
    }

    vector<string> camino;
    string actual = destino;
    while (actual != inicio) {
        camino.push_back(actual);
        actual = padre[actual];
    }
    camino.push_back(inicio);
    reverse(camino.begin(), camino.end());

    return camino;
}

int Grafo::distanciaEntre(const string& id1, const string& id2) const {
    auto camino = caminoMasCorto(id1, id2);
    if (camino.empty()) return -1;
    return camino.size() - 1;
}

vector<vector<string>> Grafo::componentesConexas() const {
    vector<vector<string>> componentes;
    unordered_set<string> visitados;

    for (const auto& par : adyacencia) {
        const string& id = par.first;
        if (visitados.find(id) != visitados.end()) continue;

        vector<string> componente;
        queue<string> cola;
        cola.push(id);
        visitados.insert(id);

        while (!cola.empty()) {
            string actual = cola.front();
            cola.pop();
            componente.push_back(actual);

            for (const auto& vecino : adyacencia.at(actual)) {
                if (visitados.find(vecino) == visitados.end()) {
                    visitados.insert(vecino);
                    cola.push(vecino);
                }
            }
        }

        if (!componente.empty()) {
            componentes.push_back(componente);
        }
    }

    return componentes;
}

int Grafo::cantidadAmistades() const {
    int total = 0;
    for (const auto& par : adyacencia) {
        total += par.second.size();
    }
    return total / 2; // Grafo no dirigido
}

double Grafo::densidad() const {
    int n = cantidadUsuarios();
    if (n < 2) return 0.0;

    int maxAristas = n * (n - 1) / 2;
    return static_cast<double>(cantidadAmistades()) / maxAristas;
}

bool Grafo::guardar(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) return false;

    // Guardar usuarios
    archivo << "USUARIOS\n";
    for (const auto& par : usuarios) {
        const Usuario& u = par.second;
        archivo << u.id << "|" << u.nombre << "|" << u.email << "|"
            << u.carrera << "|" << u.institucion << "|" << u.tipo << "\n";
    }

    // Guardar amistades
    archivo << "AMISTADES\n";
    for (const auto& par : adyacencia) {
        for (const auto& amigo : par.second) {
            if (par.first < amigo) { // Guardar solo una vez
                archivo << par.first << "|" << amigo << "\n";
            }
        }
    }

    archivo.close();
    return true;
}

bool Grafo::cargar(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return false;

    string linea;
    bool leyendoUsuarios = false;
    bool leyendoAmistades = false;

    // Limpiar datos actuales
    adyacencia.clear();
    usuarios.clear();

    while (getline(archivo, linea)) {
        if (linea == "USUARIOS") {
            leyendoUsuarios = true;
            leyendoAmistades = false;
            continue;
        }
        else if (linea == "AMISTADES") {
            leyendoUsuarios = false;
            leyendoAmistades = true;
            continue;
        }

        if (linea.empty()) continue;

        if (leyendoUsuarios) {
            stringstream ss(linea);
            string id, nombre, email, carrera, institucion, tipo;
            getline(ss, id, '|');
            getline(ss, nombre, '|');
            getline(ss, email, '|');
            getline(ss, carrera, '|');
            getline(ss, institucion, '|');
            getline(ss, tipo, '|');

            Usuario usuario(id, nombre, email, "", carrera, institucion, tipo);
            agregarUsuario(usuario);
        }
        else if (leyendoAmistades) {
            stringstream ss(linea);
            string id1, id2;
            getline(ss, id1, '|');
            getline(ss, id2, '|');
            agregarAmistad(id1, id2);
        }
    }

    archivo.close();
    return true;
}

void Grafo::imprimir() const {
    cout << "=== GRAFO DE AMISTADES ===" << endl;
    cout << "Usuarios: " << cantidadUsuarios() << endl;
    cout << "Amistades: " << cantidadAmistades() << endl;
    cout << "Densidad: " << densidad() << endl;
    cout << "Componentes conexas: " << componentesConexas().size() << endl;

    for (const auto& par : adyacencia) {
        const string& id = par.first;
        const auto& amigos = par.second;

        auto it = usuarios.find(id);
        string nombre = (it != usuarios.end()) ? it->second.nombre : id;

        cout << nombre << " -> ";
        for (const auto& amigoId : amigos) {
            auto itAmigo = usuarios.find(amigoId);
            string nombreAmigo = (itAmigo != usuarios.end()) ? itAmigo->second.nombre : amigoId;
            cout << nombreAmigo << " ";
        }
        cout << endl;
    }
    cout << "===========================" << endl;
}