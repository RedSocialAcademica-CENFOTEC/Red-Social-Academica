#ifndef MONGOAPI_H
#define MONGOAPI_H

#include "modelos.h"
#include <string>
#include <vector>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using namespace std;

class MongoAPI {
private:
    mongocxx::instance instance;
    mongocxx::client cliente;
    mongocxx::database db;
    string conexionString;

    // Colecciones
    static constexpr const char* COL_USUARIOS = "usuarios";
    static constexpr const char* COL_PUBLICACIONES = "publicaciones";
    static constexpr const char* COL_COMENTARIOS = "comentarios";
    static constexpr const char* COL_SOLICITUDES = "solicitudes";

    // Conversiones
    bsoncxx::document::value usuarioToBson(const Usuario& usuario);
    Usuario bsonToUsuario(const bsoncxx::document::view& doc);

    bsoncxx::document::value publicacionToBson(const Publicacion& pub);
    Publicacion bsonToPublicacion(const bsoncxx::document::view& doc);

    bsoncxx::document::value comentarioToBson(const Comentario& comentario);
    Comentario bsonToComentario(const bsoncxx::document::view& doc);

public:
    MongoAPI(const string& _conexionString = "mongodb://localhost:27017");
    ~MongoAPI();

    bool conectar();
    void desconectar();
    bool estaConectado() const;

    // CRUD Usuarios
    bool insertarUsuario(const Usuario& usuario);
    bool actualizarUsuario(const string& id, const Usuario& usuario);
    bool eliminarUsuario(const string& id);
    vector<Usuario> obtenerTodosUsuarios();
    Usuario obtenerUsuarioPorId(const string& id);
    vector<Usuario> buscarUsuarios(const string& campo, const string& valor);

    // CRUD Publicaciones
    bool insertarPublicacion(const Publicacion& pub);
    vector<Publicacion> obtenerTodasPublicaciones();
    vector<Publicacion> obtenerPublicacionesPorUsuario(const string& idUsuario);

    // CRUD Comentarios
    bool insertarComentario(const Comentario& comentario);
    vector<Comentario> obtenerComentariosPorPublicacion(const string& idPublicacion);

    // CRUD Solicitudes
    bool insertarSolicitud(const SolicitudAmistad& solicitud);
    vector<SolicitudAmistad> obtenerSolicitudesPendientes(const string& idUsuario);
    bool actualizarSolicitud(const string& id, const string& estado);

    // Estadísticas
    int contarUsuarios();
    int contarPublicaciones();
    int contarComentarios();
};

#endif