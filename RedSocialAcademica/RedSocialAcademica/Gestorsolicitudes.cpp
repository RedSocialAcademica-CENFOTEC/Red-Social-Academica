#include "gestorsolicitudes.h"
#include <fstream>
#include <sstream>

GestorSolicitudes::GestorSolicitudes(Grafo& _grafo) : grafo(_grafo) {}

int GestorSolicitudes::buscarPendiente(int idA, int idB) const {
    for (size_t i = 0; i < solicitudes.size(); i++) {
        const SolicitudAmistad& s = solicitudes[i];
        if (s.estado != "pendiente") continue;
        if ((s.idEmisor == idA && s.idReceptor == idB) ||
            (s.idEmisor == idB && s.idReceptor == idA)) {
            return (int)i;
        }
    }
    return -1;
}

bool GestorSolicitudes::enviarSolicitud(int idEmisor, int idReceptor) {
    if (idEmisor == idReceptor) return false;
    if (!grafo.existeUsuario(idEmisor) || !grafo.existeUsuario(idReceptor)) return false;
    if (grafo.sonAmigos(idEmisor, idReceptor)) return false; // ya son amigos
    if (buscarPendiente(idEmisor, idReceptor) != -1) return false; // ya hay una solicitud pendiente

    SolicitudAmistad nueva;
    nueva.idEmisor = idEmisor;
    nueva.idReceptor = idReceptor;
    nueva.estado = "pendiente";
    solicitudes.push_back(nueva);
    return true;
}

bool GestorSolicitudes::aceptarSolicitud(int idEmisor, int idReceptor) {
    int idx = buscarPendiente(idEmisor, idReceptor);
    if (idx == -1) return false;

    solicitudes[idx].estado = "aceptada";
    return grafo.agregarAmistad(solicitudes[idx].idEmisor, solicitudes[idx].idReceptor);
}

bool GestorSolicitudes::rechazarSolicitud(int idEmisor, int idReceptor) {
    int idx = buscarPendiente(idEmisor, idReceptor);
    if (idx == -1) return false;

    solicitudes[idx].estado = "rechazada";
    return true;
}

vector<SolicitudAmistad> GestorSolicitudes::obtenerPendientesRecibidas(int idUsuario) const {
    vector<SolicitudAmistad> resultado;
    for (const SolicitudAmistad& s : solicitudes) {
        if (s.estado == "pendiente" && s.idReceptor == idUsuario) {
            resultado.push_back(s);
        }
    }
    return resultado;
}

vector<SolicitudAmistad> GestorSolicitudes::obtenerPendientesEnviadas(int idUsuario) const {
    vector<SolicitudAmistad> resultado;
    for (const SolicitudAmistad& s : solicitudes) {
        if (s.estado == "pendiente" && s.idEmisor == idUsuario) {
            resultado.push_back(s);
        }
    }
    return resultado;
}

void GestorSolicitudes::guardarEnArchivo(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo);
    for (const SolicitudAmistad& s : solicitudes) {
        archivo << s.idEmisor << "|" << s.idReceptor << "|" << s.estado << "\n";
    }
    archivo.close();
}

void GestorSolicitudes::cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    solicitudes.clear();
    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string idEmisorStr, idReceptorStr, estado;
        getline(ss, idEmisorStr, '|');
        getline(ss, idReceptorStr, '|');
        getline(ss, estado, '|');

        SolicitudAmistad s;
        s.idEmisor = stoi(idEmisorStr);
        s.idReceptor = stoi(idReceptorStr);
        s.estado = estado;
        solicitudes.push_back(s);
    }
    archivo.close();
}