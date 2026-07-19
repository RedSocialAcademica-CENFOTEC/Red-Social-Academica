#include <iostream>
#include "modelos.h"
#include "bplustree.h"
#include "dynamichash.h"
#include "notificationqueue.h"
#include "grafo.h"
#include "gestorusuarios.h"
#include "gestorsolicitudes.h"
#include "gestorcomentarios.h"
using namespace std;

int main() {
    BPlusTree arbol(2); // orden a proposito, para forzar divisiones rapido

    int fechas[] = { 20260610, 20260601, 20260615, 20260605, 20260620, 20260603 };

    for (int i = 0; i < 6; i++) {
        Publicacion pub;
        pub.id = i + 1;
        pub.idAutor = 1;
        pub.contenido = "Publicacion de prueba " + to_string(i + 1);
        pub.fecha = fechas[i];
        arbol.insertar(pub);
    }

    arbol.imprimirHojas();

    cout << endl << "Feed entre " << formatearFecha(20260603) << " y " << formatearFecha(20260615) << ":" << endl;
    vector<Publicacion> feed = arbol.obtenerFeed(20260603, 20260615);
    for (const Publicacion& p : feed) {
        cout << "  " << formatearFecha(p.fecha) << " - " << p.contenido << endl;
    }

    cout << endl << "Eliminando publicacion #3..." << endl;
    arbol.eliminar(3);
    arbol.imprimirHojas();

    cout << endl << "=== Prueba de persistencia: Arbol B+ ===" << endl;
    arbol.guardarEnArchivo("publicaciones.dat");
    BPlusTree arbolCargado(2);
    arbolCargado.cargarDesdeArchivo("publicaciones.dat");
    cout << "Arbol cargado desde publicaciones.dat:" << endl;
    arbolCargado.imprimirHojas();

    cout << endl << "=== Prueba de Hashing Dinamico ===" << endl;
    DynamicHash hash;

    hash.insertar({ 1, "Trayce", "trayce@cenfotec.cr", "Ing. Software", "CENFOTEC", TipoUsuario::ESTUDIANTE });
    hash.insertar({ 2, "Matias", "matias@cenfotec.cr", "Ing. Software", "CENFOTEC", TipoUsuario::ESTUDIANTE });
    hash.insertar({ 3, "Mario", "mario@cenfotec.cr", "Ing. Software", "CENFOTEC", TipoUsuario::PROFESOR });
    hash.insertar({ 4, "Ana", "ana@cenfotec.cr", "Administracion", "CENFOTEC", TipoUsuario::ESTUDIANTE });
    hash.insertar({ 5, "Luis", "luis@ucr.ac.cr", "Ing. Software", "UCR", TipoUsuario::ESTUDIANTE });

    Usuario* encontrado = hash.buscarPorNombre("Matias");
    if (encontrado != nullptr) {
        cout << "Encontrado por nombre: " << encontrado->nombre << " - " << encontrado->correo << endl;
    }

    cout << "Usuarios en Ing. Software:" << endl;
    for (const Usuario& u : hash.buscarPorCarrera("Ing. Software")) {
        cout << "  " << u.nombre << endl;
    }

    cout << "Usuarios en CENFOTEC:" << endl;
    for (const Usuario& u : hash.buscarPorInstitucion("CENFOTEC")) {
        cout << "  " << u.nombre << endl;
    }

    cout << endl << "=== Prueba de persistencia: Hashing Dinamico ===" << endl;
    hash.guardarEnArchivo("usuarios.dat");
    DynamicHash hashCargado;
    hashCargado.cargarDesdeArchivo("usuarios.dat");
    cout << "Usuarios cargados desde usuarios.dat:" << endl;
    for (const Usuario& u : hashCargado.obtenerTodos()) {
        cout << "  " << u.nombre << " - " << u.correo << endl;
    }

    cout << endl << "=== Prueba de Cola de Notificaciones ===" << endl;
    NotificationQueue colaNotif;

    colaNotif.encolar({ 1, 1, "Matias comento tu publicacion", 20260610 });          // prioridad normal
    colaNotif.encolar({ 2, 1, "Nueva solicitud de amistad", 20260611 }, 5);           // prioridad alta
    colaNotif.encolar({ 3, 1, "A alguien le gusto tu publicacion", 20260612 });      // prioridad normal
    colaNotif.encolar({ 4, 1, "Alerta de seguridad", 20260613 }, 10);                 // prioridad muy alta

    cout << endl << "=== Prueba de persistencia: Cola de Notificaciones ===" << endl;
    colaNotif.guardarEnArchivo("notificaciones.dat");
    NotificationQueue colaCargada;
    colaCargada.cargarDesdeArchivo("notificaciones.dat");
    cout << "Orden cargado desde notificaciones.dat (debe respetar FIFO con prioridad):" << endl;
    while (!colaCargada.estaVacia()) {
        Notificacion n = colaCargada.desencolar();
        cout << "  " << n.mensaje << endl;
    }

    cout << "Orden de salida original (FIFO con prioridad):" << endl;
    while (!colaNotif.estaVacia()) {
        Notificacion n = colaNotif.desencolar();
        cout << "  " << n.mensaje << endl;
    }

    cout << endl << "=== Prueba de Grafo de Amistades + Usuarios + Solicitudes + Comentarios ===" << endl;
    Grafo grafo;
    GestorUsuarios gestorUsuarios(grafo);
    GestorSolicitudes gestorSolicitudes(grafo);
    GestorComentarios gestorComentarios;

    int idTrayce = gestorUsuarios.registrar("Trayce", "trayce@cenfotec.cr", "clave123",
        "Ing. Software", "CENFOTEC", TipoUsuario::ESTUDIANTE);
    int idMatias = gestorUsuarios.registrar("Matias", "matias@cenfotec.cr", "clave456",
        "Ing. Software", "CENFOTEC", TipoUsuario::ESTUDIANTE);
    int idMario = gestorUsuarios.registrar("Mario", "mario@cenfotec.cr", "clave789",
        "Ing. Software", "CENFOTEC", TipoUsuario::PROFESOR);

    cout << "Usuarios registrados con ids: " << idTrayce << ", " << idMatias << ", " << idMario << endl;

    Usuario logueado;
    if (gestorUsuarios.iniciarSesion("matias@cenfotec.cr", "clave456", logueado)) {
        cout << "Login OK para: " << logueado.nombre << endl;
    }
    if (!gestorUsuarios.iniciarSesion("matias@cenfotec.cr", "claveIncorrecta", logueado)) {
        cout << "Login con clave incorrecta fue rechazado correctamente" << endl;
    }

    gestorUsuarios.editarPerfil(idMatias, "Matias Lutz", "Ing. Software", "CENFOTEC");
    cout << "Perfil de Matias actualizado: " << grafo.obtenerUsuario(idMatias).nombre << endl;

    cout << endl << "Enviando solicitud de amistad Trayce -> Matias..." << endl;
    gestorSolicitudes.enviarSolicitud(idTrayce, idMatias);
    cout << "Son amigos antes de aceptar? " << (grafo.sonAmigos(idTrayce, idMatias) ? "si" : "no") << endl;

    gestorSolicitudes.aceptarSolicitud(idTrayce, idMatias);
    cout << "Son amigos despues de aceptar? " << (grafo.sonAmigos(idTrayce, idMatias) ? "si" : "no") << endl;

    gestorSolicitudes.enviarSolicitud(idMario, idMatias);
    gestorSolicitudes.rechazarSolicitud(idMario, idMatias);
    cout << "Son amigos Mario y Matias tras rechazo? " << (grafo.sonAmigos(idMario, idMatias) ? "si" : "no") << endl;

    gestorComentarios.agregarComentario(idMatias, 1, "Buen punto, coincido", 20260610);
    gestorComentarios.agregarComentario(idTrayce, 1, "Gracias!", 20260611);

    cout << endl << "Comentarios de la publicacion #1:" << endl;
    for (const Comentario& c : gestorComentarios.obtenerPorPublicacion(1)) {
        cout << "  [" << formatearFecha(c.fecha) << "] usuario " << c.idUsuario << ": " << c.contenido << endl;
    }

    grafo.imprimir();

    return 0;
}