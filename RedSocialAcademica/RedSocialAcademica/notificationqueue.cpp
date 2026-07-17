#include "notificationqueue.h"
#include <algorithm>

NotificationQueue::NotificationQueue(int _maxSize) : maxSize(_maxSize) {}

NotificationQueue::~NotificationQueue() {}

void NotificationQueue::encolar(const Notificacion& notificacion) {
    lock_guard<mutex> lock(mtx);
    cola.push(notificacion);
    historial.push_back(notificacion);

    // Limitar tamaño de la cola
    if (cola.size() > maxSize) {
        cola.pop();
    }
}

Notificacion NotificationQueue::desencolar() {
    lock_guard<mutex> lock(mtx);
    if (cola.empty()) {
        throw runtime_error("Cola de notificaciones vacía");
    }

    Notificacion notificacion = cola.front();
    cola.pop();
    return notificacion;
}

Notificacion NotificationQueue::verSiguiente() const {
    lock_guard<mutex> lock(mtx);
    if (cola.empty()) {
        throw runtime_error("Cola de notificaciones vacía");
    }
    return cola.front();
}

bool NotificationQueue::vacia() const {
    lock_guard<mutex> lock(mtx);
    return cola.empty();
}

int NotificationQueue::tamaño() const {
    lock_guard<mutex> lock(mtx);
    return cola.size();
}

vector<Notificacion> NotificationQueue::obtenerNoLeidas(const string& idUsuario) const {
    lock_guard<mutex> lock(mtx);
    vector<Notificacion> resultado;

    for (const auto& notif : historial) {
        if (notif.idUsuario == idUsuario && !notif.leida) {
            resultado.push_back(notif);
        }
    }

    return resultado;
}

void NotificationQueue::marcarComoLeida(const string& idNotificacion) {
    lock_guard<mutex> lock(mtx);
    for (auto& notif : historial) {
        if (notif.id == idNotificacion) {
            notif.leida = true;
            break;
        }
    }
}

void NotificationQueue::marcarTodasComoLeidas(const string& idUsuario) {
    lock_guard<mutex> lock(mtx);
    for (auto& notif : historial) {
        if (notif.idUsuario == idUsuario) {
            notif.leida = true;
        }
    }
}

void NotificationQueue::limpiar() {
    lock_guard<mutex> lock(mtx);
    while (!cola.empty()) {
        cola.pop();
    }
    historial.clear();
}

bool NotificationQueue::guardar(const string& nombreArchivo) const {
    lock_guard<mutex> lock(mtx);
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Guardar historial
    size_t tam = historial.size();
    archivo.write(reinterpret_cast<const char*>(&tam), sizeof(tam));

    for (const auto& notif : historial) {
        // Serializar notificación
        size_t len = notif.id.length();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo.write(notif.id.c_str(), len);

        len = notif.idUsuario.length();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo.write(notif.idUsuario.c_str(), len);

        len = notif.tipo.length();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo.write(notif.tipo.c_str(), len);

        len = notif.mensaje.length();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo.write(notif.mensaje.c_str(), len);

        len = notif.idReferencia.length();
        archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
        archivo.write(notif.idReferencia.c_str(), len);

        archivo.write(reinterpret_cast<const char*>(&notif.leida), sizeof(notif.leida));
    }

    archivo.close();
    return true;
}

bool NotificationQueue::cargar(const string& nombreArchivo) {
    lock_guard<mutex> lock(mtx);
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Limpiar datos actuales
    while (!cola.empty()) cola.pop();
    historial.clear();

    size_t tam;
    archivo.read(reinterpret_cast<char*>(&tam), sizeof(tam));

    for (size_t i = 0; i < tam; i++) {
        Notificacion notif;
        size_t len;

        archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
        notif.id.resize(len);
        archivo.read(&notif.id[0], len);

        archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
        notif.idUsuario.resize(len);
        archivo.read(&notif.idUsuario[0], len);

        archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
        notif.tipo.resize(len);
        archivo.read(&notif.tipo[0], len);

        archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
        notif.mensaje.resize(len);
        archivo.read(&notif.mensaje[0], len);

        archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
        notif.idReferencia.resize(len);
        archivo.read(&notif.idReferencia[0], len);

        archivo.read(reinterpret_cast<char*>(&notif.leida), sizeof(notif.leida));

        historial.push_back(notif);
    }

    archivo.close();
    return true;
}

void NotificationQueue::imprimir() const {
    lock_guard<mutex> lock(mtx);
    cout << "=== COLAS DE NOTIFICACIONES ===" << endl;
    cout << "Total en cola: " << cola.size() << endl;
    cout << "Total histórico: " << historial.size() << endl;

    queue<Notificacion> temp = cola;
    int i = 0;
    while (!temp.empty()) {
        Notificacion n = temp.front();
        temp.pop();
        cout << i++ << ". " << n.mensaje << " (leída: " << (n.leida ? "Sí" : "No") << ")" << endl;
    }
    cout << "================================" << endl;
}