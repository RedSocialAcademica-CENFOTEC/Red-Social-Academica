#pragma once
#include "modelos.h"
#include <vector>
#include <string>
using namespace std;

class GestorComentarios {
private:
    vector<Comentario> comentarios;
    int siguienteId;

public:
    GestorComentarios();

    // Devuelve el id asignado al comentario
    int agregarComentario(int idUsuario, int idPublicacion, const string& contenido, int fecha);
    bool eliminarComentario(int id);

    vector<Comentario> obtenerPorPublicacion(int idPublicacion) const;
    vector<Comentario> obtenerPorUsuario(int idUsuario) const;
    int cantidadComentarios(int idPublicacion) const;

    void guardarEnArchivo(const string& nombreArchivo) const;
    void cargarDesdeArchivo(const string& nombreArchivo);
};