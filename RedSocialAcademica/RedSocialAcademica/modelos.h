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
    int idPublicacion;     // a que publicacion pertenece
    string contenido;
    int fecha;             // formato AAAAMMDD
};
// Publicacion: la unidad que va a vivir dentro del Arbol B+
struct Publicacion {
    int id;
    int idAutor;
    string contenido;
    int fecha;                    // AAAAMMDD -> esta es la clave del arbol B+
    vector<int> idsComentarios;   // referencias, no objetos completos
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
//convierte a texto lindo solo para mostrarla la parte de la fecha 
inline string formatearFecha(int fecha) {
    int anio = fecha / 10000;
    int mes = (fecha / 100) % 100;
    int dia = fecha % 100;
    static const string meses[] = { "", "enero", "febrero", "marzo", "abril", "mayo", "junio",
                                     "julio", "agosto", "septiembre", "octubre", "noviembre", "diciembre" };
    return to_string(dia) + " de " + meses[mes] + " del " + to_string(anio);
}