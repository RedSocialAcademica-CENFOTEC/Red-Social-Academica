#include "gestorusuarios.h"

GestorUsuarios::GestorUsuarios(Grafo& _grafo) : grafo(_grafo) {
    // El siguiente id disponible arranca despues del id mas alto que ya exista
    // (por si el grafo se cargo de un archivo con usuarios previos)
    siguienteId = 1;
    for (const Usuario& u : grafo.obtenerTodosUsuarios()) {
        if (u.id >= siguienteId) siguienteId = u.id + 1;
    }
}

bool GestorUsuarios::correoEnUso(const string& correo) const {
    for (const Usuario& u : grafo.obtenerTodosUsuarios()) {
        if (u.correo == correo) return true;
    }
    return false;
}

int GestorUsuarios::registrar(const string& nombre, const string& correo, const string& contrasena,
    const string& carrera, const string& institucion, TipoUsuario tipo) {

    if (correoEnUso(correo)) return -1; // ya existe una cuenta con ese correo

    Usuario nuevo;
    nuevo.id = siguienteId;
    nuevo.nombre = nombre;
    nuevo.correo = correo;
    nuevo.carrera = carrera;
    nuevo.institucion = institucion;
    nuevo.tipo = tipo;
    nuevo.contrasena = contrasena;

    grafo.agregarUsuario(nuevo);
    siguienteId++;
    return nuevo.id;
}

bool GestorUsuarios::iniciarSesion(const string& correo, const string& contrasena, Usuario& usuarioOut) const {
    for (const Usuario& u : grafo.obtenerTodosUsuarios()) {
        if (u.correo == correo && u.contrasena == contrasena) {
            usuarioOut = u;
            return true;
        }
    }
    return false; // correo no existe o contrasena incorrecta (no distinguimos, por seguridad)
}

bool GestorUsuarios::editarPerfil(int id, const string& nuevoNombre, const string& nuevaCarrera, const string& nuevaInstitucion) {
    if (!grafo.existeUsuario(id)) return false;

    Usuario actual = grafo.obtenerUsuario(id);
    actual.nombre = nuevoNombre;
    actual.carrera = nuevaCarrera;
    actual.institucion = nuevaInstitucion;

    return grafo.actualizarUsuario(id, actual);
}

bool GestorUsuarios::cambiarCorreo(int id, const string& nuevoCorreo) {
    if (!grafo.existeUsuario(id)) return false;
    if (correoEnUso(nuevoCorreo)) return false;

    Usuario actual = grafo.obtenerUsuario(id);
    actual.correo = nuevoCorreo;
    return grafo.actualizarUsuario(id, actual);
}

bool GestorUsuarios::cambiarContrasena(int id, const string& contrasenaActual, const string& contrasenaNueva) {
    if (!grafo.existeUsuario(id)) return false;

    Usuario actual = grafo.obtenerUsuario(id);
    if (actual.contrasena != contrasenaActual) return false; // no coincide la contrasena actual

    actual.contrasena = contrasenaNueva;
    return grafo.actualizarUsuario(id, actual);
}