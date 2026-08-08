#include "mongoapi.h"
#include "json.hpp"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QByteArray>
#include <iostream>

using json = nlohmann::json;

// ==================== HTTP de bajo nivel (Qt Network) ====================
//
// Todo el proyecto llama a estos metodos de forma sincronica (desde botones
// de la GUI, uno a la vez), asi que envolvemos cada request en su propio
// QEventLoop para bloquear hasta tener respuesta, en vez de reescribir el
// resto del proyecto a un estilo asincronico con senales/slots.

MongoAPI::MongoAPI(const string& _baseUrl) : baseUrl(_baseUrl) {}

string MongoAPI::colUsuarios() const { return string(PREFIJO) + "usuarios"; }
string MongoAPI::colPublicaciones() const { return string(PREFIJO) + "publicaciones"; }
string MongoAPI::colComentarios() const { return string(PREFIJO) + "comentarios"; }
string MongoAPI::colSolicitudes() const { return string(PREFIJO) + "solicitudes"; }

static string ejecutarRequestSincrona(QNetworkAccessManager& manager, QNetworkRequest req,
    const string& metodo, const string& jsonBody) {
    QEventLoop loop;
    QNetworkReply* reply = nullptr;

    if (metodo == "GET") {
        reply = manager.get(req);
    }
    else if (metodo == "POST") {
        reply = manager.post(req, QByteArray::fromStdString(jsonBody));
    }
    else if (metodo == "PUT") {
        reply = manager.put(req, QByteArray::fromStdString(jsonBody));
    }
    else if (metodo == "DELETE") {
        // QNetworkAccessManager no tiene un metodo deleteResource() con
        // cuerpo, asi que armamos el request DELETE manualmente.
        reply = manager.sendCustomRequest(req, "DELETE", QByteArray::fromStdString(jsonBody));
    }
    else {
        return "";
    }

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    // Timeout de seguridad de 15s, por si la API no responde
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeout.start(15000);

    loop.exec();

    string respuesta;
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        respuesta = reply->readAll().toStdString();
    }
    else if (reply->isFinished()) {
        cerr << "Error " << metodo << " a MongoAPI: " << reply->errorString().toStdString() << endl;
        // Igual intentamos leer el cuerpo: algunas APIs mandan detalle del error en JSON
        respuesta = reply->readAll().toStdString();
    }
    else {
        cerr << "Timeout esperando respuesta de MongoAPI (" << metodo << ")" << endl;
    }
    reply->deleteLater();
    return respuesta;
}

string MongoAPI::httpGet(const string& queryString) const {
    QNetworkAccessManager manager;
    QUrl url(QString::fromStdString(baseUrl));
    url.setQuery(QString::fromStdString(queryString));
    QNetworkRequest req(url);
    return ejecutarRequestSincrona(manager, req, "GET", "");
}

string MongoAPI::httpEnviar(const string& metodo, const string& jsonBody) const {
    QNetworkAccessManager manager;
    QNetworkRequest req(QUrl(QString::fromStdString(baseUrl)));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return ejecutarRequestSincrona(manager, req, metodo, jsonBody);
}

bool MongoAPI::conectar() {
    // Consulta liviana para confirmar que la API responde algo
    string resp = httpGet("coleccion=" + colUsuarios());
    conectada = !resp.empty();
    if (!conectada) {
        cerr << "No se pudo contactar la API en " << baseUrl << endl;
    }
    return conectada;
}

bool MongoAPI::estaConectado() const { return conectada; }

// ==================== Conversiones JSON ====================

static json usuarioToJson(const Usuario& u) {
    return json{
        {"id", u.id}, {"nombre", u.nombre}, {"correo", u.correo},
        {"carrera", u.carrera}, {"institucion", u.institucion},
        {"tipo", static_cast<int>(u.tipo)}, {"contrasena", u.contrasena}
    };
}

static Usuario jsonToUsuario(const json& j) {
    Usuario u;
    u.id = j.value("id", 0);
    u.nombre = j.value("nombre", "");
    u.correo = j.value("correo", "");
    u.carrera = j.value("carrera", "");
    u.institucion = j.value("institucion", "");
    u.tipo = static_cast<TipoUsuario>(j.value("tipo", 0));
    u.contrasena = j.value("contrasena", "");
    return u;
}

static json publicacionToJson(const Publicacion& p) {
    return json{
        {"id", p.id}, {"idAutor", p.idAutor}, {"contenido", p.contenido},
        {"fecha", p.fecha}, {"idsComentarios", p.idsComentarios}
    };
}

static Publicacion jsonToPublicacion(const json& j) {
    Publicacion p;
    p.id = j.value("id", 0);
    p.idAutor = j.value("idAutor", 0);
    p.contenido = j.value("contenido", "");
    p.fecha = j.value("fecha", 0);
    if (j.contains("idsComentarios") && j["idsComentarios"].is_array()) {
        p.idsComentarios = j["idsComentarios"].get<vector<int>>();
    }
    return p;
}

static json comentarioToJson(const Comentario& c) {
    return json{
        {"id", c.id}, {"idUsuario", c.idUsuario}, {"idPublicacion", c.idPublicacion},
        {"contenido", c.contenido}, {"fecha", c.fecha}
    };
}

static Comentario jsonToComentario(const json& j) {
    Comentario c;
    c.id = j.value("id", 0);
    c.idUsuario = j.value("idUsuario", 0);
    c.idPublicacion = j.value("idPublicacion", 0);
    c.contenido = j.value("contenido", "");
    c.fecha = j.value("fecha", 0);
    return c;
}

static json solicitudToJson(const SolicitudAmistad& s) {
    return json{ {"idEmisor", s.idEmisor}, {"idReceptor", s.idReceptor}, {"estado", s.estado} };
}

static SolicitudAmistad jsonToSolicitud(const json& j) {
    SolicitudAmistad s;
    s.idEmisor = j.value("idEmisor", 0);
    s.idReceptor = j.value("idReceptor", 0);
    s.estado = j.value("estado", "pendiente");
    return s;
}

// Intenta parsear la respuesta como un array de documentos; la API puede
// envolver los resultados en distintas formas segun el endpoint del
// profesor, asi que contemplamos las variantes mas comunes.
static vector<json> extraerArreglo(const string& respuestaCruda) {
    vector<json> items;
    if (respuestaCruda.empty()) return items;
    try {
        json j = json::parse(respuestaCruda);
        if (j.is_array()) {
            for (auto& item : j) items.push_back(item);
        }
        else if (j.is_object()) {
            if (j.contains("documentos") && j["documentos"].is_array()) {
                for (auto& item : j["documentos"]) items.push_back(item);
            }
            else if (j.contains("data") && j["data"].is_array()) {
                for (auto& item : j["data"]) items.push_back(item);
            }
            else if (j.contains("resultado") && j["resultado"].is_array()) {
                for (auto& item : j["resultado"]) items.push_back(item);
            }
            else if (j.contains("id")) {
                items.push_back(j); // un solo documento real
            }
            // si no matchea ninguna forma conocida (ej. solo {"coleccion":.., "total":0}
            // sin "documentos"), no se agrega nada: coleccion vacia, no un documento basura.
        }
    }
    catch (const json::parse_error& e) {
        cerr << "Respuesta de MongoAPI no es JSON valido: " << e.what() << endl;
    }
    return items;
}

// ==================== CRUD Usuarios ====================

bool MongoAPI::insertarUsuario(const Usuario& usuario) {
    json body = { {"coleccion", colUsuarios()}, {"datos", usuarioToJson(usuario)} };
    string resp = httpEnviar("POST", body.dump());
    return !resp.empty();
}

bool MongoAPI::actualizarUsuario(int id, const Usuario& usuario) {
    json body = {
        {"coleccion", colUsuarios()},
        {"filtro", {{"id", id}}},
        {"datos", usuarioToJson(usuario)}
    };
    string resp = httpEnviar("PUT", body.dump());
    return !resp.empty();
}

bool MongoAPI::eliminarUsuario(int id) {
    json body = { {"coleccion", colUsuarios()}, {"filtro", {{"id", id}}} };
    string resp = httpEnviar("DELETE", body.dump());
    return !resp.empty();
}

vector<Usuario> MongoAPI::obtenerTodosUsuarios() {
    vector<Usuario> resultado;
    string resp = httpGet("coleccion=" + colUsuarios());
    for (const json& j : extraerArreglo(resp)) resultado.push_back(jsonToUsuario(j));
    return resultado;
}

// ==================== CRUD Publicaciones ====================

bool MongoAPI::insertarPublicacion(const Publicacion& pub) {
    json body = { {"coleccion", colPublicaciones()}, {"datos", publicacionToJson(pub)} };
    string resp = httpEnviar("POST", body.dump());
    return !resp.empty();
}

bool MongoAPI::eliminarPublicacion(int id) {
    json body = { {"coleccion", colPublicaciones()}, {"filtro", {{"id", id}}} };
    string resp = httpEnviar("DELETE", body.dump());
    return !resp.empty();
}

vector<Publicacion> MongoAPI::obtenerTodasPublicaciones() {
    vector<Publicacion> resultado;
    string resp = httpGet("coleccion=" + colPublicaciones());
    for (const json& j : extraerArreglo(resp)) resultado.push_back(jsonToPublicacion(j));
    return resultado;
}

// ==================== CRUD Comentarios ====================

bool MongoAPI::insertarComentario(const Comentario& comentario) {
    json body = { {"coleccion", colComentarios()}, {"datos", comentarioToJson(comentario)} };
    string resp = httpEnviar("POST", body.dump());
    return !resp.empty();
}

vector<Comentario> MongoAPI::obtenerTodosComentarios() {
    vector<Comentario> resultado;
    string resp = httpGet("coleccion=" + colComentarios());
    for (const json& j : extraerArreglo(resp)) resultado.push_back(jsonToComentario(j));
    return resultado;
}

// ==================== CRUD Solicitudes ====================

bool MongoAPI::insertarSolicitud(const SolicitudAmistad& solicitud) {
    json body = { {"coleccion", colSolicitudes()}, {"datos", solicitudToJson(solicitud)} };
    string resp = httpEnviar("POST", body.dump());
    return !resp.empty();
}

bool MongoAPI::actualizarSolicitud(int idEmisor, int idReceptor, const string& estado) {
    json body = {
        {"coleccion", colSolicitudes()},
        {"filtro", {{"idEmisor", idEmisor}, {"idReceptor", idReceptor}}},
        {"datos", {{"estado", estado}}}
    };
    string resp = httpEnviar("PUT", body.dump());
    return !resp.empty();
}

vector<SolicitudAmistad> MongoAPI::obtenerTodasSolicitudes() {
    vector<SolicitudAmistad> resultado;
    string resp = httpGet("coleccion=" + colSolicitudes());
    for (const json& j : extraerArreglo(resp)) resultado.push_back(jsonToSolicitud(j));
    return resultado;
}