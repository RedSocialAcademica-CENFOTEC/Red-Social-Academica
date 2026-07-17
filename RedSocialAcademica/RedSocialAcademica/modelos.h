#pragma once
#include <string>
#include <vector>
using namespace std;

// Tipo de usuario 
enum class TipoUsuario { ESTUDIANTE, PROFESOR, ADMINISTRADOR };

struct Usuario {
    int id;
    string nombre;
    string correo;
    string carrera;       // vacio si es profesor/admin
    string institucion;
    TipoUsuario tipo;
};

struct Comentario {
    int id;
    int idUsuario;
    string contenido;
    int fecha;             // formato AAAAMMDD
};

// Publicacion: la unidad que va a vivir dentro del Arbol B+
struct Publicacion {
    int id;
    int idAutor;
    string contenido;
    int fecha;              // AAAAMMDD -> esta es la clave del arbol B+
    vector<Comentario> comentarios;
};

struct SolicitudAmistad {
    int idEmisor;
    int idReceptor;
    string estado;          // "pendiente", "aceptada", "rechazada"
};

struct Notificacion {
    int id;
    int idUsuarioDestino;
    string mensaje;
    int fecha;
};