#include "mongoapi.h"
#include <mongocxx/exception/exception.hpp>

MongoAPI::MongoAPI(const string& _conexionString)
    : conexionString(_conexionString) {
}

MongoAPI::~MongoAPI() {
    desconectar();
}

bool MongoAPI::conectar() {
    try {
        mongocxx::uri uri(conexionString);
        cliente = mongocxx::client(uri);
        db = cliente["red_social_academica"];
        return true;
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error conectando a MongoDB: " << e.what() << endl;
        return false;
    }
}

void MongoAPI::desconectar() {
    // MongoDB C++ driver maneja la desconexión automáticamente
}

bool MongoAPI::estaConectado() const {
    try {
        return cliente && cliente["admin"].run_command(bsoncxx::builder::stream::document{} << "ping" << 1 << bsoncxx::builder::stream::finalize);
    }
    catch (...) {
        return false;
    }
}

bsoncxx::document::value MongoAPI::usuarioToBson(const Usuario& usuario) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;

    document doc;
    doc << "id" << usuario.id
        << "nombre" << usuario.nombre
        << "email" << usuario.email
        << "contrasena" << usuario.contrasena
        << "carrera" << usuario.carrera
        << "institucion" << usuario.institucion
        << "tipo" << usuario.tipo
        << "activo" << usuario.activo
        << "fechaRegistro" << usuario.fechaRegistro.toString();

    return doc << finalize;
}

Usuario MongoAPI::bsonToUsuario(const bsoncxx::document::view& doc) {
    Usuario usuario;
    usuario.id = doc["id"].get_string().value.to_string();
    usuario.nombre = doc["nombre"].get_string().value.to_string();
    usuario.email = doc["email"].get_string().value.to_string();
    usuario.contrasena = doc["contrasena"].get_string().value.to_string();
    usuario.carrera = doc["carrera"].get_string().value.to_string();
    usuario.institucion = doc["institucion"].get_string().value.to_string();
    usuario.tipo = doc["tipo"].get_string().value.to_string();
    usuario.activo = doc["activo"].get_bool().value;

    // Parsear fecha
    string fechaStr = doc["fechaRegistro"].get_string().value.to_string();
    // Implementar parsing de fecha si es necesario

    return usuario;
}

bool MongoAPI::insertarUsuario(const Usuario& usuario) {
    if (!estaConectado()) return false;

    try {
        auto collection = db[COL_USUARIOS];
        auto doc = usuarioToBson(usuario);
        auto resultado = collection.insert_one(doc.view());
        return resultado && resultado->result().inserted_count() > 0;
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error insertando usuario: " << e.what() << endl;
        return false;
    }
}

bool MongoAPI::actualizarUsuario(const string& id, const Usuario& usuario) {
    if (!estaConectado()) return false;

    try {
        auto collection = db[COL_USUARIOS];
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id
            << bsoncxx::builder::stream::finalize;
        auto update = bsoncxx::builder::stream::document{}
            << "$set" << bsoncxx::builder::stream::open_document
            << "nombre" << usuario.nombre
            << "email" << usuario.email
            << "carrera" << usuario.carrera
            << "institucion" << usuario.institucion
            << "tipo" << usuario.tipo
            << "activo" << usuario.activo
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;

        auto resultado = collection.update_one(filter.view(), update.view());
        return resultado && resultado->result().modified_count() > 0;
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error actualizando usuario: " << e.what() << endl;
        return false;
    }
}

bool MongoAPI::eliminarUsuario(const string& id) {
    if (!estaConectado()) return false;

    try {
        auto collection = db[COL_USUARIOS];
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id
            << bsoncxx::builder::stream::finalize;
        auto resultado = collection.delete_one(filter.view());
        return resultado && resultado->result().deleted_count() > 0;
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error eliminando usuario: " << e.what() << endl;
        return false;
    }
}

vector<Usuario> MongoAPI::obtenerTodosUsuarios() {
    vector<Usuario> usuarios;
    if (!estaConectado()) return usuarios;

    try {
        auto collection = db[COL_USUARIOS];
        auto cursor = collection.find({});

        for (auto&& doc : cursor) {
            usuarios.push_back(bsonToUsuario(doc));
        }
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error obteniendo usuarios: " << e.what() << endl;
    }

    return usuarios;
}

Usuario MongoAPI::obtenerUsuarioPorId(const string& id) {
    if (!estaConectado()) return Usuario();

    try {
        auto collection = db[COL_USUARIOS];
        auto filter = bsoncxx::builder::stream::document{}
            << "id" << id
            << bsoncxx::builder::stream::finalize;
        auto resultado = collection.find_one(filter.view());

        if (resultado) {
            return bsonToUsuario(resultado->view());
        }
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error obteniendo usuario: " << e.what() << endl;
    }

    return Usuario();
}

vector<Usuario> MongoAPI::buscarUsuarios(const string& campo, const string& valor) {
    vector<Usuario> usuarios;
    if (!estaConectado()) return usuarios;

    try {
        auto collection = db[COL_USUARIOS];
        auto filter = bsoncxx::builder::stream::document{}
            << campo << valor
            << bsoncxx::builder::stream::finalize;
        auto cursor = collection.find(filter.view());

        for (auto&& doc : cursor) {
            usuarios.push_back(bsonToUsuario(doc));
        }
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error buscando usuarios: " << e.what() << endl;
    }

    return usuarios;
}

// Implementaciones similares para las demás funciones CRUD...

bsoncxx::document::value MongoAPI::publicacionToBson(const Publicacion& pub) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;

    document doc;
    doc << "id" << pub.id
        << "idUsuario" << pub.idUsuario
        << "contenido" << pub.contenido
        << "fecha" << pub.fecha.toString()
        << "likes" << [&](bsoncxx::builder::stream::array_context<> arr) {
        for (const auto& like : pub.likes) {
            arr << like;
        }
        }
        << "comentariosIds" << [&](bsoncxx::builder::stream::array_context<> arr) {
        for (const auto& comentario : pub.comentariosIds) {
            arr << comentario;
        }
        };

    return doc << finalize;
}

Publicacion MongoAPI::bsonToPublicacion(const bsoncxx::document::view& doc) {
    Publicacion pub;
    pub.id = doc["id"].get_string().value.to_string();
    pub.idUsuario = doc["idUsuario"].get_string().value.to_string();
    pub.contenido = doc["contenido"].get_string().value.to_string();

    // Parsear fecha si es necesario
    string fechaStr = doc["fecha"].get_string().value.to_string();

    // Cargar likes
    auto likesArr = doc["likes"].get_array().value;
    for (auto&& elem : likesArr) {
        pub.likes.push_back(elem.get_string().value.to_string());
    }

    // Cargar comentarios
    auto comentariosArr = doc["comentariosIds"].get_array().value;
    for (auto&& elem : comentariosArr) {
        pub.comentariosIds.push_back(elem.get_string().value.to_string());
    }

    return pub;
}

bool MongoAPI::insertarPublicacion(const Publicacion& pub) {
    if (!estaConectado()) return false;

    try {
        auto collection = db[COL_PUBLICACIONES];
        auto doc = publicacionToBson(pub);
        auto resultado = collection.insert_one(doc.view());
        return resultado && resultado->result().inserted_count() > 0;
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error insertando publicación: " << e.what() << endl;
        return false;
    }
}

vector<Publicacion> MongoAPI::obtenerTodasPublicaciones() {
    vector<Publicacion> publicaciones;
    if (!estaConectado()) return publicaciones;

    try {
        auto collection = db[COL_PUBLICACIONES];
        auto cursor = collection.find({});

        for (auto&& doc : cursor) {
            publicaciones.push_back(bsonToPublicacion(doc));
        }
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error obteniendo publicaciones: " << e.what() << endl;
    }

    return publicaciones;
}

vector<Publicacion> MongoAPI::obtenerPublicacionesPorUsuario(const string& idUsuario) {
    vector<Publicacion> publicaciones;
    if (!estaConectado()) return publicaciones;

    try {
        auto collection = db[COL_PUBLICACIONES];
        auto filter = bsoncxx::builder::stream::document{}
            << "idUsuario" << idUsuario
            << bsoncxx::builder::stream::finalize;
        auto cursor = collection.find(filter.view());

        for (auto&& doc : cursor) {
            publicaciones.push_back(bsonToPublicacion(doc));
        }
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error obteniendo publicaciones por usuario: " << e.what() << endl;
    }

    return publicaciones;
}

// Implementar el resto de funciones CRUD para comentarios y solicitudes...

int MongoAPI::contarUsuarios() {
    if (!estaConectado()) return 0;
    try {
        auto collection = db[COL_USUARIOS];
        return collection.count_documents({});
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error contando usuarios: " << e.what() << endl;
        return 0;
    }
}

int MongoAPI::contarPublicaciones() {
    if (!estaConectado()) return 0;
    try {
        auto collection = db[COL_PUBLICACIONES];
        return collection.count_documents({});
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error contando publicaciones: " << e.what() << endl;
        return 0;
    }
}

int MongoAPI::contarComentarios() {
    if (!estaConectado()) return 0;
    try {
        auto collection = db[COL_COMENTARIOS];
        return collection.count_documents({});
    }
    catch (const mongocxx::exception& e) {
        cerr << "Error contando comentarios: " << e.what() << endl;
        return 0;
    }
}