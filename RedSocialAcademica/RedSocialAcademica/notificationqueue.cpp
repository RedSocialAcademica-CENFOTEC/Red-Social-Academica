#include "notificationqueue.h"
#include <fstream>
#include <sstream>

NodoNotificacion::NodoNotificacion(const Notificacion& n, int prio)
    : dato(n), prioridad(prio), siguiente(nullptr) {
}

NotificationQueue::NotificationQueue() : frente(nullptr), final(nullptr), cantidad(0) {}

void NotificationQueue::encolar(const Notificacion& n, int prioridad) {
    NodoNotificacion* nuevo = new NodoNotificacion(n, prioridad);
    cantidad++;

    if (frente == nullptr) {
        frente = nuevo;
        final = nuevo;
        return;
    }

    // Si la nueva notificacion es mas urgente que TODO lo que hay, se va directo al frente
    if (prioridad > frente->prioridad) {
        nuevo->siguiente = frente;
        frente = nuevo;
        return;
    }

    // Si no, la insertamos despues del ultimo nodo que tenga prioridad igual o mayor
    // (asi las de igual prioridad mantienen el orden FIFO entre si)
    NodoNotificacion* actual = frente;
    while (actual->siguiente != nullptr && actual->siguiente->prioridad >= prioridad) {
        actual = actual->siguiente;
    }

    nuevo->siguiente = actual->siguiente;
    actual->siguiente = nuevo;

    if (nuevo->siguiente == nullptr) {
        final = nuevo;
    }
}

Notificacion NotificationQueue::desencolar() {
    Notificacion resultado = frente->dato;
    NodoNotificacion* viejo = frente;
    frente = frente->siguiente;
    if (frente == nullptr) {
        final = nullptr;
    }
    delete viejo;
    cantidad--;
    return resultado;
}

Notificacion NotificationQueue::verSiguiente() const {
    return frente->dato;
}

bool NotificationQueue::estaVacia() const {
    return frente == nullptr;
}

int NotificationQueue::tamano() const {
    return cantidad;
}

/*desencolar() y verSiguiente() asumen que la cola no está vacía. Si los llamás con la cola vacía, 
frente es nullptr y el programa se rompe (acceso a memoria inválida). Por eso siempre hay que chequear estaVacia() antes de llamarlos
*/

void NotificationQueue::guardarEnArchivo(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo);
    NodoNotificacion* actual = frente;
    while (actual != nullptr) {
        archivo << actual->dato.id << "|" << actual->dato.idUsuarioDestino << "|"
            << actual->dato.mensaje << "|" << actual->dato.fecha << "|" << actual->prioridad << endl;
        actual = actual->siguiente;
    }
    archivo.close();
}

void NotificationQueue::cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string campo;
        Notificacion n;
        int prioridad;

        getline(ss, campo, '|'); n.id = stoi(campo);
        getline(ss, campo, '|'); n.idUsuarioDestino = stoi(campo);
        getline(ss, campo, '|'); n.mensaje = campo;
        getline(ss, campo, '|'); n.fecha = stoi(campo);
        getline(ss, campo, '|'); prioridad = stoi(campo);

        encolar(n, prioridad);
    }
    archivo.close();
}