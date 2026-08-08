#ifndef MONGOAPI_H
#define MONGOAPI_H

#include "modelos.h"
#include <string>
#include <vector>

using namespace std;

// Cliente REST contra la API PHP/MongoDB del profesor:
//   https://paginas-web-cr.com/Api/apis/mongodb.php
// POST   -> insertar   {"coleccion":"...", "datos": {...}}
// GET    -> consultar  ?coleccion=...&campo=valor
// PUT    -> actualizar {"coleccion":"...", "filtro": {...}, "datos": {...}}
// DELETE -> eliminar   {"coleccion":"...", "filtro": {...}}
//
// Importante: usamos SIEMPRE nuestro propio campo "id" (entero, asignado
// localmente por los Gestores) como clave de negocio dentro del documento,
// nunca el _id que genera Mongo. Asi el resto del proyecto (Grafo,
// GestorUsuarios, etc.) no cambia su forma de identificar entidades.
class MongoAPI {
private:
    string baseUrl;
    bool conectada = false;

    // Nombres reales de coleccion en Mongo, con el prefijo asignado
    static constexpr const char* PREFIJO = "RS_TGONZALEZ_MLUTZ_";
    string colUsuarios() const;
    string colPublicaciones() const;
    string colComentarios() const;
    string colSolicitudes() const;

    // HTTP de bajo nivel (libcurl)
    string httpGet(const string& queryString) const;
    string httpEnviar(const string& metodo, const string& jsonBody) const;

public:
    explicit MongoAPI(const string& _baseUrl = "https://paginas-web-cr.com/Api/apis/mongodb.php");

    bool conectar();     // hace una consulta de prueba; true si la API responde
    bool estaConectado() const;

    // CRUD Usuarios
    bool insertarUsuario(const Usuario& usuario);
    bool actualizarUsuario(int id, const Usuario& usuario);
    bool eliminarUsuario(int id);
    vector<Usuario> obtenerTodosUsuarios();

    // CRUD Publicaciones
    bool insertarPublicacion(const Publicacion& pub);
    bool eliminarPublicacion(int id);
    vector<Publicacion> obtenerTodasPublicaciones();

    // CRUD Comentarios
    bool insertarComentario(const Comentario& comentario);
    vector<Comentario> obtenerTodosComentarios();

    // CRUD Solicitudes
    bool insertarSolicitud(const SolicitudAmistad& solicitud);
    bool actualizarSolicitud(int idEmisor, int idReceptor, const string& estado);
    vector<SolicitudAmistad> obtenerTodasSolicitudes();
};

#endif
