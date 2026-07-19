#pragma once
#include "modelos.h"
#include "grafo.h"
#include <string>
using namespace std;

class GestorUsuarios {
private:
    Grafo& grafo;
    int siguienteId;

    bool correoEnUso(const string& correo) const;

public:
    explicit GestorUsuarios(Grafo& _grafo);

    // Devuelve el id asignado, o -1 si el correo ya estaba en uso
    int registrar(const string& nombre, const string& correo, const string& contrasena,
        const string& carrera, const string& institucion, TipoUsuario tipo);

    // Si el login es correcto, llena usuarioOut y devuelve true
    bool iniciarSesion(const string& correo, const string& contrasena, Usuario& usuarioOut) const;

    // Edita nombre/carrera/institucion. El correo y la contrasena tienen sus propios metodos
    // porque ameritan validaciones distintas (correo unico, confirmar contrasena actual)
    bool editarPerfil(int id, const string& nuevoNombre, const string& nuevaCarrera, const string& nuevaInstitucion);
    bool cambiarCorreo(int id, const string& nuevoCorreo);
    bool cambiarContrasena(int id, const string& contrasenaActual, const string& contrasenaNueva);
};