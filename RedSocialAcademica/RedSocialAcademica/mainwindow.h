#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

#include "modelos.h"
#include "grafo.h"
#include "gestorusuarios.h"
#include "gestorsolicitudes.h"
#include "gestorcomentarios.h"
#include "bplustree.h"
#include "dynamichash.h"
#include "notificationqueue.h"

// Ventana principal: agrupa todas las estructuras del backend y una pestana
// por cada modulo del sistema. Nada de logica de negocio nueva vive aca -
// esta clase solo conecta la UI con los metodos que ya existen en Grafo,
// GestorUsuarios, GestorSolicitudes, GestorComentarios, BPlusTree, DynamicHash
// y NotificationQueue.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private:
    // --- Backend (las mismas estructuras de datos del proyecto de consola) ---
    Grafo grafo;
    GestorUsuarios gestorUsuarios;
    GestorSolicitudes gestorSolicitudes;
    GestorComentarios gestorComentarios;
    BPlusTree arbol;
    DynamicHash hash;
    NotificationQueue colaNotif;

    int usuarioActualId = -1; // -1 = nadie logueado
    int siguientePublicacionId = 1;
    int siguienteNotifId = 1;

    QTabWidget* tabs;

    // --- Pestana: Login / Registro ---
    QWidget* crearTabLogin();
    QLineEdit* loginCorreo;
    QLineEdit* loginClave;
    QLabel* loginEstado;

    QLineEdit* regNombre;
    QLineEdit* regCorreo;
    QLineEdit* regClave;
    QLineEdit* regCarrera;
    QLineEdit* regInstitucion;
    QComboBox* regTipo;
    QLabel* regEstado;

    void hacerLogin();
    void hacerRegistro();
    void hacerLogout();
    void actualizarEstadoSesion();

    // --- Pestana: Perfil ---
    QWidget* crearTabPerfil();
    QLabel* perfilInfo;
    QLineEdit* perfilNombre;
    QLineEdit* perfilCarrera;
    QLineEdit* perfilInstitucion;
    QLabel* perfilEstado;
    void refrescarPerfil();
    void guardarPerfil();

    // --- Pestana: Amigos ---
    QWidget* crearTabAmigos();
    QListWidget* listaUsuarios;
    QListWidget* listaAmigos;
    QListWidget* listaPendientesRecibidas;
    QListWidget* listaSugerencias;
    QLabel* amigosEstado;
    void refrescarAmigos();
    void enviarSolicitudSeleccionada();
    void aceptarSolicitudSeleccionada();
    void rechazarSolicitudSeleccionada();

    // --- Pestana: Publicaciones ---
    QWidget* crearTabPublicaciones();
    QLineEdit* pubContenido;
    QLineEdit* pubFecha;
    QListWidget* listaPublicaciones;
    QLineEdit* feedDesde;
    QLineEdit* feedHasta;
    QLabel* pubEstado;
    void crearPublicacion();
    void refrescarFeedCompleto();
    void buscarFeedPorRango();
    void eliminarPublicacionSeleccionada();

    // --- Pestana: Busqueda de usuarios (Hashing Dinamico) ---
    QWidget* crearTabBusqueda();
    QLineEdit* buscarTexto;
    QComboBox* buscarPor;
    QListWidget* resultadosBusqueda;
    void ejecutarBusqueda();

    // --- Pestana: Notificaciones ---
    QWidget* crearTabNotificaciones();
    QLabel* notifSiguiente;
    QLabel* notifCantidad;
    void refrescarNotificaciones();
    void marcarNotificacionLeida();
    void generarNotificacionDePrueba();

    // --- Pestana: Comentarios ---
    QWidget* crearTabComentarios();
    QComboBox* comentPublicacionId;
    QLineEdit* comentTexto;
    QListWidget* listaComentarios;
    void refrescarComboPublicaciones();
    void agregarComentario();
    void refrescarComentarios();

    // --- Helper compartido ---
    void notificar(int idUsuarioDestino, const std::string& mensaje, int prioridad = 0);
};