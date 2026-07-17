#ifndef MODELOS_H
#define MODELOS_H

#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

// Estructura para manejar fechas
struct Fecha {
    int anio, mes, dia;
    int hora, minuto, segundo;

    Fecha(int a = 2024, int m = 1, int d = 1, int h = 0, int min = 0, int s = 0)
        : anio(a), mes(m), dia(d), hora(h), minuto(min), segundo(s) {
    }

    // Comparadores para el árbol B+
    bool operator<(const Fecha& other) const {
        if (anio != other.anio) return anio < other.anio;
        if (mes != other.mes) return mes < other.mes;
        if (dia != other.dia) return dia < other.dia;
        if (hora != other.hora) return hora < other.hora;
        if (minuto != other.minuto) return minuto < other.minuto;
        return segundo < other.segundo;
    }

    bool operator==(const Fecha& other) const {
        return anio == other.anio && mes == other.mes && dia == other.dia &&
            hora == other.hora && minuto == other.minuto && segundo == other.segundo;
    }

    string toString() const {
        stringstream ss;
        ss << setw(4) << setfill('0') << anio << "-"
            << setw(2) << setfill('0') << mes << "-"
            << setw(2) << setfill('0') << dia << " "
            << setw(2) << setfill('0') << hora << ":"
            << setw(2) << setfill('0') << minuto << ":"
            << setw(2) << setfill('0') << segundo;
        return ss.str();
    }

    static Fecha ahora() {
        auto now = chrono::system_clock::now();
        time_t tiempo = chrono::system_clock::to_time_t(now);
        tm* tiempo_local = localtime(&tiempo);
        return Fecha(tiempo_local->tm_year + 1900,
            tiempo_local->tm_mon + 1,
            tiempo_local->tm_mday,
            tiempo_local->tm_hour,
            tiempo_local->tm_min,
            tiempo_local->tm_sec);
    }
};

// Clase Usuario
class Usuario {
public:
    string id;
    string nombre;
    string email;
    string contrasena;
    string carrera;
    string institucion;
    string tipo; // "estudiante", "docente", "admin"
    Fecha fechaRegistro;
    bool activo;

    Usuario() : activo(true) {}

    Usuario(string _id, string _nombre, string _email, string _contrasena,
        string _carrera, string _institucion, string _tipo)
        : id(_id), nombre(_nombre), email(_email), contrasena(_contrasena),
        carrera(_carrera), institucion(_institucion), tipo(_tipo),
        fechaRegistro(Fecha::ahora()), activo(true) {
    }

    string toString() const {
        return "ID: " + id + ", Nombre: " + nombre + ", Email: " + email +
            ", Carrera: " + carrera + ", Institución: " + institucion +
            ", Tipo: " + tipo;
    }
};

// Clase Publicacion
class Publicacion {
public:
    string id;
    string idUsuario;
    string contenido;
    Fecha fecha;
    vector<string> likes;
    vector<string> comentariosIds;

    Publicacion() {}

    Publicacion(string _id, string _idUsuario, string _contenido)
        : id(_id), idUsuario(_idUsuario), contenido(_contenido),
        fecha(Fecha::ahora()) {
    }

    bool operator<(const Publicacion& other) const {
        return fecha < other.fecha;
    }

    bool operator>(const Publicacion& other) const {
        return other.fecha < fecha;
    }

    string toString() const {
        return "ID: " + id + ", Usuario: " + idUsuario +
            ", Fecha: " + fecha.toString() +
            ", Contenido: " + contenido.substr(0, 50) + "...";
    }
};

// Clase Comentario
class Comentario {
public:
    string id;
    string idPublicacion;
    string idUsuario;
    string contenido;
    Fecha fecha;

    Comentario() {}

    Comentario(string _id, string _idPublicacion, string _idUsuario, string _contenido)
        : id(_id), idPublicacion(_idPublicacion), idUsuario(_idUsuario),
        contenido(_contenido), fecha(Fecha::ahora()) {
    }
};

// Clase SolicitudAmistad
class SolicitudAmistad {
public:
    string id;
    string idEmisor;
    string idReceptor;
    string estado; // "pendiente", "aceptada", "rechazada"
    Fecha fecha;

    SolicitudAmistad() {}

    SolicitudAmistad(string _id, string _idEmisor, string _idReceptor)
        : id(_id), idEmisor(_idEmisor), idReceptor(_idReceptor),
        estado("pendiente"), fecha(Fecha::ahora()) {
    }
};

// Clase Notificacion
class Notificacion {
public:
    string id;
    string idUsuario;
    string tipo; // "solicitud_amistad", "like", "comentario", "mensaje"
    string mensaje;
    string idReferencia; // ID del objeto relacionado
    Fecha fecha;
    bool leida;

    Notificacion() : leida(false) {}

    Notificacion(string _id, string _idUsuario, string _tipo, string _mensaje, string _idRef)
        : id(_id), idUsuario(_idUsuario), tipo(_tipo), mensaje(_mensaje),
        idReferencia(_idRef), fecha(Fecha::ahora()), leida(false) {
    }
};

#endif