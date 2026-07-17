#include <iostream>
#include <string>
#include <limits>
#include "modelos.h"
#include "bplustree.h"
#include "dynamichash.h"
#include "grafo.h"
#include "notificationqueue.h"

using namespace std;

// Variables globales para el sistema
BPlusTree<Publicacion> arbolPublicaciones(4);
DynamicHash hashUsuarios(4);
Grafo redAmistades;
NotificationQueue colaNotificaciones(100);

// Funciones auxiliares
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void esperarEnter() {
    cout << "\nPresiona Enter para continuar...";
    limpiarBuffer();
    cin.get();
}

string generarID() {
    static int contador = 1;
    return "ID" + to_string(contador++);
}

// Funciones del menú
void menuPrincipal();
void menuUsuarios();
void menuAmistades();
void menuPublicaciones();
void menuBusqueda();
void menuNotificaciones();
void menuEstadisticas();

void menuPrincipal() {
    int opcion;
    do {
        system("clear"); // En Windows usa "cls"
        cout << "=== RED SOCIAL ACADÉMICA ===" << endl;
        cout << "1. Gestión de Usuarios" << endl;
        cout << "2. Gestión de Amistades" << endl;
        cout << "3. Publicaciones" << endl;
        cout << "4. Búsqueda" << endl;
        cout << "5. Notificaciones" << endl;
        cout << "6. Estadísticas" << endl;
        cout << "7. Guardar datos" << endl;
        cout << "8. Cargar datos" << endl;
        cout << "9. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;
        limpiarBuffer();

        switch (opcion) {
        case 1: menuUsuarios(); break;
        case 2: menuAmistades(); break;
        case 3: menuPublicaciones(); break;
        case 4: menuBusqueda(); break;
        case 5: menuNotificaciones(); break;
        case 6: menuEstadisticas(); break;
        case 7:
            cout << "Guardando datos...\n";
            redAmistades.guardar("amistades.dat");
            arbolPublicaciones.guardar("publicaciones.dat");
            hashUsuarios.guardar("usuarios_hash.dat");
            colaNotificaciones.guardar("notificaciones.dat");
            cout << "Datos guardados correctamente.\n";
            esperarEnter();
            break;
        case 8:
            cout << "Cargando datos...\n";
            redAmistades.cargar("amistades.dat");
            arbolPublicaciones.cargar("publicaciones.dat");
            hashUsuarios.cargar("usuarios_hash.dat");
            colaNotificaciones.cargar("notificaciones.dat");
            cout << "Datos cargados correctamente.\n";
            esperarEnter();
            break;
        case 9:
            cout << "Saliendo del sistema...\n";
            break;
        default:
            cout << "Opción no válida.\n";
            esperarEnter();
        }
    } while (opcion != 9);
}

void menuUsuarios() {
    int opcion;
    do {
        system("clear");
        cout << "=== GESTIÓN DE USUARIOS ===" << endl;
        cout << "1. Registrar usuario" << endl;
        cout << "2. Ver todos los usuarios" << endl;
        cout << "3. Buscar usuario por ID" << endl;
        cout << "4. Eliminar usuario" << endl;
        cout << "5. Volver" << endl;
        cout << "Seleccione: ";
        cin >> opcion;
        limpiarBuffer();

        if (opcion == 1) {
            string nombre, email, password, carrera, institucion, tipo;
            cout << "Nombre: "; getline(cin, nombre);
            cout << "Email: "; getline(cin, email);
            cout << "Contraseña: "; getline(cin, password);
            cout << "Carrera: "; getline(cin, carrera);
            cout << "Institución: "; getline(cin, institucion);
            cout << "Tipo (estudiante/docente/admin): "; getline(cin, tipo);

            string id = generarID();
            Usuario nuevo(id, nombre, email, password, carrera, institucion, tipo);
            hashUsuarios.insertar(nuevo);
            redAmistades.agregarUsuario(nuevo);
            cout << "Usuario registrado con ID: " << id << endl;
            esperarEnter();
        }
        else if (opcion == 2) {
            cout << "\n=== USUARIOS REGISTRADOS ===" << endl;
            // No tenemos un método directo para listar todos, implementar si es necesario
            cout << "Función en desarrollo..." << endl;
            esperarEnter();
        }
        else if (opcion == 3) {
            string id;
            cout << "ID del usuario: "; getline(cin, id);
            if (redAmistades.existeUsuario(id)) {
                cout << redAmistades.obtenerUsuario(id).toString() << endl;
            }
            else {
                cout << "Usuario no encontrado." << endl;
            }
            esperarEnter();
        }
        else if (opcion == 4) {
            string id;
            cout << "ID del usuario a eliminar: "; getline(cin, id);
            if (redAmistades.existeUsuario(id)) {
                redAmistades.eliminarUsuario(id);
                cout << "Usuario eliminado." << endl;
            }
            else {
                cout << "Usuario no encontrado." << endl;
            }
            esperarEnter();
        }
    } while (opcion != 5);
}

void menuAmistades() {
    int opcion;
    do {
        system("clear");
        cout << "=== GESTIÓN DE AMISTADES ===" << endl;
        cout << "1. Agregar amistad" << endl;
        cout << "2. Eliminar amistad" << endl;
        cout << "3. Ver amigos de un usuario" << endl;
        cout << "4. Sugerir amigos" << endl;
        cout << "5. Ver amigos en común" << endl;
        cout << "6. Volver" << endl;
        cout << "Seleccione: ";
        cin >> opcion;
        limpiarBuffer();

        if (opcion == 1) {
            string id1, id2;
            cout << "ID del primer usuario: "; getline(cin, id1);
            cout << "ID del segundo usuario: "; getline(cin, id2);
            if (redAmistades.agregarAmistad(id1, id2)) {
                cout << "Amistad agregada correctamente." << endl;
                colaNotificaciones.encolar(Notificacion(
                    generarID(), id2, "solicitud_amistad",
                    "Has recibido una nueva solicitud de amistad", id1
                ));
            }
            else {
                cout << "No se pudo agregar la amistad." << endl;
            }
            esperarEnter();
        }
        else if (opcion == 2) {
            string id1, id2;
            cout << "ID del primer usuario: "; getline(cin, id1);
            cout << "ID del segundo usuario: "; getline(cin, id2);
            if (redAmistades.eliminarAmistad(id1, id2)) {
                cout << "Amistad eliminada." << endl;
            }
            else {
                cout << "No se pudo eliminar la amistad." << endl;
            }
            esperarEnter();
        }
        else if (opcion == 3) {
            string id;
            cout << "ID del usuario: "; getline(cin, id);
            auto amigos = redAmistades.obtenerAmigos(id);
            cout << "Amigos (" << amigos.size() << "):" << endl;
            for (const auto& amigoId : amigos) {
                cout << "- " << amigoId << endl;
            }
            esperarEnter();
        }
        else if (opcion == 4) {
            string id;
            cout << "ID del usuario: "; getline(cin, id);
            auto sugerencias = redAmistades.sugerirAmigos(id);
            cout << "Sugerencias de amistad:" << endl;
            for (const auto& sug : sugerencias) {
                cout << "- " << sug << endl;
            }
            esperarEnter();
        }
        else if (opcion == 5) {
            string id1, id2;
            cout << "ID del primer usuario: "; getline(cin, id1);
            cout << "ID del segundo usuario: "; getline(cin, id2);
            auto comunes = redAmistades.amigosEnComun(id1, id2);
            cout << "Amigos en común (" << comunes.size() << "):" << endl;
            for (const auto& amigo : comunes) {
                cout << "- " << amigo << endl;
            }
            esperarEnter();
        }
    } while (opcion != 6);
}

void menuPublicaciones() {
    int opcion;
    do {
        system("clear");
        cout << "=== PUBLICACIONES ===" << endl;
        cout << "1. Crear publicación" << endl;
        cout << "2. Ver todas las publicaciones" << endl;
        cout << "3. Buscar publicaciones por rango de fechas" << endl;
        cout << "4. Eliminar publicación" << endl;
        cout << "5. Volver" << endl;
        cout << "Seleccione: ";
        cin >> opcion;
        limpiarBuffer();

        if (opcion == 1) {
            string idUsuario, contenido;
            cout << "ID del usuario: "; getline(cin, idUsuario);
            cout << "Contenido: "; getline(cin, contenido);
            Publicacion pub(generarID(), idUsuario, contenido);
            arbolPublicaciones.insertar(pub);
            cout << "Publicación creada." << endl;
            esperarEnter();
        }
        else if (opcion == 2) {
            auto pubs = arbolPublicaciones.obtenerTodos();
            cout << "Publicaciones (" << pubs.size() << "):" << endl;
            for (const auto& pub : pubs) {
                cout << pub.toString() << endl;
            }
            esperarEnter();
        }
        else if (opcion == 3) {
            cout << "Buscar por rango de fechas (formato básico):" << endl;
            // Implementación simplificada
            auto pubs = arbolPublicaciones.obtenerTodos();
            cout << "Mostrando todas las publicaciones:" << endl;
            for (const auto& pub : pubs) {
                cout << pub.toString() << endl;
            }
            esperarEnter();
        }
        else if (opcion == 4) {
            string id;
            cout << "ID de la publicación a eliminar: "; getline(cin, id);
            // Necesitarías implementar búsqueda por ID en el árbol
            cout << "Función en desarrollo..." << endl;
            esperarEnter();
        }
    } while (opcion != 5);
}

void menuBusqueda() {
    int opcion;
    do {
        system("clear");
        cout << "=== BÚSQUEDA ===" << endl;
        cout << "1. Buscar por nombre" << endl;
        cout << "2. Buscar por carrera" << endl;
        cout << "3. Buscar por institución" << endl;
        cout << "4. Volver" << endl;
        cout << "Seleccione: ";
        cin >> opcion;
        limpiarBuffer();

        string valor;
        vector<Usuario> resultados;

        if (opcion == 1) {
            cout << "Nombre: "; getline(cin, valor);
            resultados = hashUsuarios.buscarPorNombre(valor);
        }
        else if (opcion == 2) {
            cout << "Carrera: "; getline(cin, valor);
            resultados = hashUsuarios.buscarPorCarrera(valor);
        }
        else if (opcion == 3) {
            cout << "Institución: "; getline(cin, valor);
            resultados = hashUsuarios.buscarPorInstitucion(valor);
        }

        if (opcion >= 1 && opcion <= 3) {
            cout << "Resultados (" << resultados.size() << "):" << endl;
            for (const auto& usuario : resultados) {
                cout << usuario.toString() << endl;
            }
            esperarEnter();
        }
    } while (opcion != 4);
}

void menuNotificaciones() {
    int opcion;
    do {
        system("clear");
        cout << "=== NOTIFICACIONES ===" << endl;
        cout << "1. Ver notificaciones no leídas" << endl;
        cout << "2. Marcar todas como leídas" << endl;
        cout << "3. Ver historial completo" << endl;
        cout << "4. Volver" << endl;
        cout << "Seleccione: ";
        cin >> opcion;
        limpiarBuffer();

        if (opcion == 1) {
            string idUsuario;
            cout << "ID del usuario: "; getline(cin, idUsuario);
            auto notif = colaNotificaciones.obtenerNoLeidas(idUsuario);
            cout << "Notificaciones no leídas (" << notif.size() << "):" << endl;
            for (const auto& n : notif) {
                cout << "- " << n.mensaje << endl;
            }
            esperarEnter();
        }
        else if (opcion == 2) {
            string idUsuario;
            cout << "ID del usuario: "; getline(cin, idUsuario);
            colaNotificaciones.marcarTodasComoLeidas(idUsuario);
            cout << "Todas las notificaciones marcadas como leídas." << endl;
            esperarEnter();
        }
        else if (opcion == 3) {
            colaNotificaciones.imprimir();
            esperarEnter();
        }
    } while (opcion != 4);
}

void menuEstadisticas() {
    system("clear");
    cout << "=== ESTADÍSTICAS DEL SISTEMA ===" << endl;
    cout << "Usuarios registrados: " << redAmistades.cantidadUsuarios() << endl;
    cout << "Amistades totales: " << redAmistades.cantidadAmistades() << endl;
    cout << "Densidad de la red: " << redAmistades.densidad() << endl;
    cout << "Publicaciones: " << arbolPublicaciones.size() << endl;
    cout << "Componentes conexas: " << redAmistades.componentesConexas().size() << endl;
    cout << "Notificaciones pendientes: " << colaNotificaciones.tamaño() << endl;
    esperarEnter();
}

int main() {
    cout << "=== RED SOCIAL ACADÉMICA ===" << endl;
    cout << "Inicializando sistema..." << endl;

    // Cargar datos si existen
    if (redAmistades.cargar("amistades.dat")) {
        cout << "Datos cargados correctamente." << endl;
    }
    else {
        cout << "No se encontraron datos guardados. Iniciando sistema nuevo." << endl;
    }

    menuPrincipal();

    cout << "¡Hasta luego!" << endl;
    return 0;
}