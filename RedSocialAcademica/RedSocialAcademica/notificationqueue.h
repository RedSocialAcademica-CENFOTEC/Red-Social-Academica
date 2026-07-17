#ifndef NOTIFICATIONQUEUE_H
#define NOTIFICATIONQUEUE_H

#include "modelos.h"
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>

using namespace std;

class NotificationQueue {
private:
    queue<Notificacion> cola;
    vector<Notificacion> historial;
    int maxSize;
    mutable mutex mtx;

public:
    NotificationQueue(int _maxSize = 100);
    ~NotificationQueue();

    // Operaciones básicas
    void encolar(const Notificacion& notificacion);
    Notificacion desencolar();
    Notificacion verSiguiente() const;
    bool vacia() const;
    int tamaño() const;

    // Gestión de notificaciones
    vector<Notificacion> obtenerNoLeidas(const string& idUsuario) const;
    void marcarComoLeida(const string& idNotificacion);
    void marcarTodasComoLeidas(const string& idUsuario);
    void limpiar();

    // Persistencia
    bool guardar(const string& nombreArchivo) const;
    bool cargar(const string& nombreArchivo);

    void imprimir() const;
};

#endif