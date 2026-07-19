#pragma once
#include "modelos.h"
#include "grafo.h"
#include <vector>
#include <string>
using namespace std;

class GestorSolicitudes {
private:
    vector<SolicitudAmistad> solicitudes;
    Grafo& grafo;

    // Busca una solicitud pendiente entre estos dos ids (en cualquier direccion).
    // Devuelve el indice en 'solicitudes', o -1 si no hay ninguna pendiente.
    int buscarPendiente(int idA, int idB) const;

public:
    explicit GestorSolicitudes(Grafo& _grafo);

    bool enviarSolicitud(int idEmisor, int idReceptor);
    bool aceptarSolicitud(int idEmisor, int idReceptor);
    bool rechazarSolicitud(int idEmisor, int idReceptor);

    vector<SolicitudAmistad> obtenerPendientesRecibidas(int idUsuario) const;
    vector<SolicitudAmistad> obtenerPendientesEnviadas(int idUsuario) const;

    void guardarEnArchivo(const string& nombreArchivo) const;
    void cargarDesdeArchivo(const string& nombreArchivo);
};