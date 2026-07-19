#include "gestorcomentarios.h"
#include <fstream>
#include <sstream>

GestorComentarios::GestorComentarios() : siguienteId(1) {}

int GestorComentarios::agregarComentario(int idUsuario, int idPublicacion, const string& contenido, int fecha) {
    Comentario nuevo;
    nuevo.id = siguienteId;
    nuevo.idUsuario = idUsuario;
    nuevo.idPublicacion = idPublicacion;
    nuevo.contenido = contenido;
    nuevo.fecha = fecha;

    comentarios.push_back(nuevo);
    siguienteId++;
    return nuevo.id;
}

bool GestorComentarios::eliminarComentario(int id) {
    for (size_t i = 0; i < comentarios.size(); i++) {
        if (comentarios[i].id == id) {
            comentarios.erase(comentarios.begin() + i);
            return true;
        }
    }
    return false;
}

vector<Comentario> GestorComentarios::obtenerPorPublicacion(int idPublicacion) const {
    vector<Comentario> resultado;
    for (const Comentario& c : comentarios) {
        if (c.idPublicacion == idPublicacion) resultado.push_back(c);
    }
    return resultado;
}

vector<Comentario> GestorComentarios::obtenerPorUsuario(int idUsuario) const {
    vector<Comentario> resultado;
    for (const Comentario& c : comentarios) {
        if (c.idUsuario == idUsuario) resultado.push_back(c);
    }
    return resultado;
}

int GestorComentarios::cantidadComentarios(int idPublicacion) const {
    int total = 0;
    for (const Comentario& c : comentarios) {
        if (c.idPublicacion == idPublicacion) total++;
    }
    return total;
}

void GestorComentarios::guardarEnArchivo(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo);
    for (const Comentario& c : comentarios) {
        archivo << c.id << "|" << c.idUsuario << "|" << c.idPublicacion << "|"
            << c.contenido << "|" << c.fecha << "\n";
    }
    archivo.close();
}

void GestorComentarios::cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    comentarios.clear();
    string linea;
    int maxId = 0;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string campo;
        Comentario c;

        getline(ss, campo, '|'); c.id = stoi(campo);
        getline(ss, campo, '|'); c.idUsuario = stoi(campo);
        getline(ss, campo, '|'); c.idPublicacion = stoi(campo);
        getline(ss, campo, '|'); c.contenido = campo;
        getline(ss, campo, '|'); c.fecha = stoi(campo);

        comentarios.push_back(c);
        if (c.id > maxId) maxId = c.id;
    }
    archivo.close();
    siguienteId = maxId + 1;
}