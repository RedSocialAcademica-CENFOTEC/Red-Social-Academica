#include <iostream>
#include "modelos.h"
#include "bplustree.h"
#include "dynamichash.h"
#include "notificationqueue.h"
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

    cout << endl << "=== Prueba de Cola de Notificaciones ===" << endl;
    NotificationQueue colaNotif;

    colaNotif.encolar({ 1, 1, "Matias comento tu publicacion", 20260610 });          // prioridad normal
    colaNotif.encolar({ 2, 1, "Nueva solicitud de amistad", 20260611 }, 5);           // prioridad alta
    colaNotif.encolar({ 3, 1, "A alguien le gusto tu publicacion", 20260612 });      // prioridad normal
    colaNotif.encolar({ 4, 1, "Alerta de seguridad", 20260613 }, 10);                 // prioridad muy alta

    cout << "Orden de salida (FIFO con prioridad):" << endl;
    while (!colaNotif.estaVacia()) {
        Notificacion n = colaNotif.desencolar();
        cout << "  " << n.mensaje << endl;
    }

    return 0;
}