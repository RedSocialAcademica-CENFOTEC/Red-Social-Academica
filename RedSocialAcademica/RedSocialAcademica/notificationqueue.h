/* la Cola FIFO. La vamos a implementar con nodos enlazados
* es una lista enlazada con dos punteros — frente (por donde sale el próximo, FIFO real) y final (por donde entra lo nuevo)
*/

#pragma once
#include "modelos.h"
using namespace std;

struct NodoNotificacion {
    Notificacion dato;
    int prioridad;          // 0 = normal, mas alto = mas urgente
    NodoNotificacion* siguiente;

    NodoNotificacion(const Notificacion& n, int prio);
};

class NotificationQueue {
private:
    NodoNotificacion* frente;   // por aca sale el proximo (FIFO)
    NodoNotificacion* final;    // por aca entra lo nuevo
    int cantidad;

public:
    NotificationQueue();
    void encolar(const Notificacion& n, int prioridad = 0);
    Notificacion desencolar();
    Notificacion verSiguiente() const;
    bool estaVacia() const;
    int tamano() const;
    void guardarEnArchivo(const string& nombreArchivo) const;
    void cargarDesdeArchivo(const string& nombreArchivo);
};