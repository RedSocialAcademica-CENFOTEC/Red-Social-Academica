#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QDate>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), gestorUsuarios(grafo), gestorSolicitudes(grafo) {

    setWindowTitle("Red Social Academica - CENFOTEC");
    resize(1000, 650);

    tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    tabs->addTab(crearTabLogin(), "Iniciar sesion / Registro");
    tabs->addTab(crearTabPerfil(), "Mi perfil");
    tabs->addTab(crearTabAmigos(), "Amigos");
    tabs->addTab(crearTabPublicaciones(), "Publicaciones");
    tabs->addTab(crearTabBusqueda(), "Buscar usuarios");
    tabs->addTab(crearTabNotificaciones(), "Notificaciones");
    tabs->addTab(crearTabComentarios(), "Comentarios");

    // Datos de demo, iguales a los que ya probamos en consola, para que la
    // ventana no arranque vacia. Se registran via GestorUsuarios (que ya
    // valida correo unico) y ademas se indexan en el hash para que la
    // pestana de Busqueda tenga algo para encontrar desde el arranque.
    struct DemoUsuario { string nombre, correo, clave, carrera, institucion; TipoUsuario tipo; };
    vector<DemoUsuario> demo = {
        {"Trayce", "trayce@cenfotec.cr", "clave123", "Ing. Software", "CENFOTEC", TipoUsuario::ESTUDIANTE},
        {"Matias", "matias@cenfotec.cr", "clave456", "Ing. Software", "CENFOTEC", TipoUsuario::ESTUDIANTE},
        {"Mario",  "mario@cenfotec.cr",  "clave789", "Ing. Software", "CENFOTEC", TipoUsuario::PROFESOR},
        {"Ana",    "ana@cenfotec.cr",    "clave000", "Administracion", "CENFOTEC", TipoUsuario::ESTUDIANTE},
        {"Luis",   "luis@ucr.ac.cr",     "clave111", "Ing. Software", "UCR", TipoUsuario::ESTUDIANTE},
    };
    for (const auto& d : demo) {
        int id = gestorUsuarios.registrar(d.nombre, d.correo, d.clave, d.carrera, d.institucion, d.tipo);
        if (id != -1) hash.insertar(grafo.obtenerUsuario(id));
    }

    connect(tabs, &QTabWidget::currentChanged, this, [this](int) {
        refrescarPerfil();
        refrescarAmigos();
        refrescarFeedCompleto();
        refrescarNotificaciones();
        refrescarComboPublicaciones();
        refrescarComentarios();
        });

    actualizarEstadoSesion();
}

void MainWindow::notificar(int idUsuarioDestino, const std::string& mensaje, int prioridad) {
    Notificacion n;
    n.id = siguienteNotifId++;
    n.idUsuarioDestino = idUsuarioDestino;
    n.mensaje = mensaje;
    QDate hoy = QDate::currentDate();
    n.fecha = hoy.year() * 10000 + hoy.month() * 100 + hoy.day();
    colaNotif.encolar(n, prioridad);
}

// ==================== LOGIN / REGISTRO ====================

QWidget* MainWindow::crearTabLogin() {
    QWidget* pagina = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(pagina);

    // --- Grupo de login ---
    QGroupBox* grupoLogin = new QGroupBox("Iniciar sesion");
    QFormLayout* formLogin = new QFormLayout();
    loginCorreo = new QLineEdit();
    loginClave = new QLineEdit();
    loginClave->setEchoMode(QLineEdit::Password);
    QPushButton* botonLogin = new QPushButton("Iniciar sesion");
    loginEstado = new QLabel();
    formLogin->addRow("Correo:", loginCorreo);
    formLogin->addRow("Contrasena:", loginClave);
    formLogin->addRow(botonLogin);
    formLogin->addRow(loginEstado);
    grupoLogin->setLayout(formLogin);
    connect(botonLogin, &QPushButton::clicked, this, &MainWindow::hacerLogin);

    // --- Grupo de registro ---
    QGroupBox* grupoRegistro = new QGroupBox("Crear cuenta");
    QFormLayout* formRegistro = new QFormLayout();
    regNombre = new QLineEdit();
    regCorreo = new QLineEdit();
    regClave = new QLineEdit();
    regClave->setEchoMode(QLineEdit::Password);
    regCarrera = new QLineEdit();
    regInstitucion = new QLineEdit();
    regTipo = new QComboBox();
    regTipo->addItem("Estudiante");   // indice 0 = TipoUsuario::ESTUDIANTE
    regTipo->addItem("Profesor");     // indice 1 = TipoUsuario::PROFESOR
    regTipo->addItem("Administrador"); // indice 2 = TipoUsuario::ADMINISTRADOR
    QPushButton* botonRegistro = new QPushButton("Registrarme");
    regEstado = new QLabel();
    formRegistro->addRow("Nombre:", regNombre);
    formRegistro->addRow("Correo:", regCorreo);
    formRegistro->addRow("Contrasena:", regClave);
    formRegistro->addRow("Carrera:", regCarrera);
    formRegistro->addRow("Institucion:", regInstitucion);
    formRegistro->addRow("Tipo:", regTipo);
    formRegistro->addRow(botonRegistro);
    formRegistro->addRow(regEstado);
    grupoRegistro->setLayout(formRegistro);
    connect(botonRegistro, &QPushButton::clicked, this, &MainWindow::hacerRegistro);

    layout->addWidget(grupoLogin);
    layout->addWidget(grupoRegistro);
    return pagina;
}

void MainWindow::hacerLogin() {
    Usuario u;
    bool ok = gestorUsuarios.iniciarSesion(loginCorreo->text().toStdString(),
        loginClave->text().toStdString(), u);
    if (ok) {
        usuarioActualId = u.id;
        loginEstado->setText("Bienvenido, " + QString::fromStdString(u.nombre));
        loginEstado->setStyleSheet("color: green;");
        actualizarEstadoSesion();
        tabs->setCurrentIndex(1); // saltar directo a Mi perfil
    }
    else {
        loginEstado->setText("Correo o contrasena incorrectos");
        loginEstado->setStyleSheet("color: red;");
    }
}

void MainWindow::hacerRegistro() {
    if (regNombre->text().isEmpty() || regCorreo->text().isEmpty() || regClave->text().isEmpty()) {
        regEstado->setText("Nombre, correo y contrasena son obligatorios");
        regEstado->setStyleSheet("color: red;");
        return;
    }

    TipoUsuario tipo = (TipoUsuario)regTipo->currentIndex();
    int id = gestorUsuarios.registrar(regNombre->text().toStdString(), regCorreo->text().toStdString(),
        regClave->text().toStdString(), regCarrera->text().toStdString(),
        regInstitucion->text().toStdString(), tipo);

    if (id == -1) {
        regEstado->setText("Ese correo ya esta registrado");
        regEstado->setStyleSheet("color: red;");
        return;
    }

    hash.insertar(grafo.obtenerUsuario(id));
    regEstado->setText("Cuenta creada con id " + QString::number(id) + ". Ya podes iniciar sesion.");
    regEstado->setStyleSheet("color: green;");
    regNombre->clear(); regCorreo->clear(); regClave->clear(); regCarrera->clear(); regInstitucion->clear();
}

void MainWindow::hacerLogout() {
    usuarioActualId = -1;
    actualizarEstadoSesion();
    tabs->setCurrentIndex(0);
}

void MainWindow::actualizarEstadoSesion() {
    bool logueado = (usuarioActualId != -1);
    for (int i = 1; i < tabs->count(); i++) {
        tabs->setTabEnabled(i, logueado);
    }
    if (logueado) {
        refrescarPerfil();
        refrescarAmigos();
        refrescarComboPublicaciones();
    }
}

// ==================== PERFIL ====================

QWidget* MainWindow::crearTabPerfil() {
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    perfilInfo = new QLabel();
    QFormLayout* form = new QFormLayout();
    perfilNombre = new QLineEdit();
    perfilCarrera = new QLineEdit();
    perfilInstitucion = new QLineEdit();
    form->addRow("Nombre:", perfilNombre);
    form->addRow("Carrera:", perfilCarrera);
    form->addRow("Institucion:", perfilInstitucion);

    QPushButton* botonGuardar = new QPushButton("Guardar cambios");
    QPushButton* botonLogout = new QPushButton("Cerrar sesion");
    perfilEstado = new QLabel();

    layout->addWidget(perfilInfo);
    layout->addLayout(form);
    layout->addWidget(botonGuardar);
    layout->addWidget(perfilEstado);
    layout->addStretch();
    layout->addWidget(botonLogout);

    connect(botonGuardar, &QPushButton::clicked, this, &MainWindow::guardarPerfil);
    connect(botonLogout, &QPushButton::clicked, this, &MainWindow::hacerLogout);

    return pagina;
}

void MainWindow::refrescarPerfil() {
    if (usuarioActualId == -1) return;
    Usuario u = grafo.obtenerUsuario(usuarioActualId);
    QString tipoTexto = u.tipo == TipoUsuario::ESTUDIANTE ? "Estudiante" :
        u.tipo == TipoUsuario::PROFESOR ? "Profesor" : "Administrador";
    perfilInfo->setText(QString("Id: %1  |  Correo: %2  |  Tipo: %3")
        .arg(u.id).arg(QString::fromStdString(u.correo)).arg(tipoTexto));
    perfilNombre->setText(QString::fromStdString(u.nombre));
    perfilCarrera->setText(QString::fromStdString(u.carrera));
    perfilInstitucion->setText(QString::fromStdString(u.institucion));
}

void MainWindow::guardarPerfil() {
    if (usuarioActualId == -1) return;
    gestorUsuarios.editarPerfil(usuarioActualId, perfilNombre->text().toStdString(),
        perfilCarrera->text().toStdString(), perfilInstitucion->text().toStdString());
    perfilEstado->setText("Perfil actualizado");
    perfilEstado->setStyleSheet("color: green;");
    refrescarPerfil();
}

// ==================== AMIGOS ====================

QWidget* MainWindow::crearTabAmigos() {
    QWidget* pagina = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(pagina);

    QVBoxLayout* colIzq = new QVBoxLayout();
    colIzq->addWidget(new QLabel("Todos los usuarios:"));
    listaUsuarios = new QListWidget();
    colIzq->addWidget(listaUsuarios);
    QPushButton* botonEnviar = new QPushButton("Enviar solicitud de amistad");
    colIzq->addWidget(botonEnviar);

    QVBoxLayout* colMedio = new QVBoxLayout();
    colMedio->addWidget(new QLabel("Solicitudes pendientes recibidas:"));
    listaPendientesRecibidas = new QListWidget();
    colMedio->addWidget(listaPendientesRecibidas);
    QHBoxLayout* botonesSolicitud = new QHBoxLayout();
    QPushButton* botonAceptar = new QPushButton("Aceptar");
    QPushButton* botonRechazar = new QPushButton("Rechazar");
    botonesSolicitud->addWidget(botonAceptar);
    botonesSolicitud->addWidget(botonRechazar);
    colMedio->addLayout(botonesSolicitud);
    colMedio->addWidget(new QLabel("Sugerencias de amistad:"));
    listaSugerencias = new QListWidget();
    colMedio->addWidget(listaSugerencias);

    QVBoxLayout* colDer = new QVBoxLayout();
    colDer->addWidget(new QLabel("Mis amigos:"));
    listaAmigos = new QListWidget();
    colDer->addWidget(listaAmigos);
    amigosEstado = new QLabel();
    colDer->addWidget(amigosEstado);

    layout->addLayout(colIzq);
    layout->addLayout(colMedio);
    layout->addLayout(colDer);

    connect(botonEnviar, &QPushButton::clicked, this, &MainWindow::enviarSolicitudSeleccionada);
    connect(botonAceptar, &QPushButton::clicked, this, &MainWindow::aceptarSolicitudSeleccionada);
    connect(botonRechazar, &QPushButton::clicked, this, &MainWindow::rechazarSolicitudSeleccionada);

    return pagina;
}

void MainWindow::refrescarAmigos() {
    if (usuarioActualId == -1) return;

    listaUsuarios->clear();
    for (const Usuario& u : grafo.obtenerTodosUsuarios()) {
        if (u.id == usuarioActualId) continue;
        QString etiqueta = QString::fromStdString(u.nombre) + (grafo.sonAmigos(usuarioActualId, u.id) ? "  (ya son amigos)" : "");
        QListWidgetItem* item = new QListWidgetItem(etiqueta);
        item->setData(Qt::UserRole, u.id);
        listaUsuarios->addItem(item);
    }

    listaAmigos->clear();
    for (int idAmigo : grafo.obtenerAmigos(usuarioActualId)) {
        listaAmigos->addItem(QString::fromStdString(grafo.obtenerUsuario(idAmigo).nombre));
    }

    listaPendientesRecibidas->clear();
    for (const SolicitudAmistad& s : gestorSolicitudes.obtenerPendientesRecibidas(usuarioActualId)) {
        QListWidgetItem* item = new QListWidgetItem("De: " + QString::fromStdString(grafo.obtenerUsuario(s.idEmisor).nombre));
        item->setData(Qt::UserRole, s.idEmisor);
        listaPendientesRecibidas->addItem(item);
    }

    listaSugerencias->clear();
    for (int idSugerido : grafo.sugerirAmigos(usuarioActualId, 5)) {
        listaSugerencias->addItem(QString::fromStdString(grafo.obtenerUsuario(idSugerido).nombre) + "  (amigo de un amigo)");
    }
}

void MainWindow::enviarSolicitudSeleccionada() {
    QListWidgetItem* item = listaUsuarios->currentItem();
    if (!item) { amigosEstado->setText("Selecciona primero un usuario de la lista"); return; }

    int idDestino = item->data(Qt::UserRole).toInt();
    bool ok = gestorSolicitudes.enviarSolicitud(usuarioActualId, idDestino);
    if (ok) {
        notificar(idDestino, grafo.obtenerUsuario(usuarioActualId).nombre + " te envio una solicitud de amistad", 5);
        amigosEstado->setText("Solicitud enviada");
        amigosEstado->setStyleSheet("color: green;");
    }
    else {
        amigosEstado->setText("No se pudo enviar (ya son amigos o ya hay una solicitud pendiente)");
        amigosEstado->setStyleSheet("color: red;");
    }
    refrescarAmigos();
}

void MainWindow::aceptarSolicitudSeleccionada() {
    QListWidgetItem* item = listaPendientesRecibidas->currentItem();
    if (!item) return;
    int idEmisor = item->data(Qt::UserRole).toInt();
    gestorSolicitudes.aceptarSolicitud(idEmisor, usuarioActualId);
    notificar(idEmisor, grafo.obtenerUsuario(usuarioActualId).nombre + " acepto tu solicitud de amistad", 3);
    refrescarAmigos();
}

void MainWindow::rechazarSolicitudSeleccionada() {
    QListWidgetItem* item = listaPendientesRecibidas->currentItem();
    if (!item) return;
    int idEmisor = item->data(Qt::UserRole).toInt();
    gestorSolicitudes.rechazarSolicitud(idEmisor, usuarioActualId);
    refrescarAmigos();
}

// ==================== PUBLICACIONES ====================

QWidget* MainWindow::crearTabPublicaciones() {
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    QGroupBox* grupoNueva = new QGroupBox("Nueva publicacion");
    QFormLayout* formNueva = new QFormLayout();
    pubContenido = new QLineEdit();
    pubFecha = new QLineEdit();
    pubFecha->setPlaceholderText("AAAAMMDD, ej. 20260718");
    QPushButton* botonPublicar = new QPushButton("Publicar");
    formNueva->addRow("Contenido:", pubContenido);
    formNueva->addRow("Fecha:", pubFecha);
    formNueva->addRow(botonPublicar);
    grupoNueva->setLayout(formNueva);

    QGroupBox* grupoFeed = new QGroupBox("Feed por rango de fechas");
    QHBoxLayout* filaFeed = new QHBoxLayout();
    feedDesde = new QLineEdit(); feedDesde->setPlaceholderText("Desde AAAAMMDD");
    feedHasta = new QLineEdit(); feedHasta->setPlaceholderText("Hasta AAAAMMDD");
    QPushButton* botonFeed = new QPushButton("Buscar en rango");
    QPushButton* botonTodas = new QPushButton("Ver todas");
    filaFeed->addWidget(feedDesde);
    filaFeed->addWidget(feedHasta);
    filaFeed->addWidget(botonFeed);
    filaFeed->addWidget(botonTodas);
    grupoFeed->setLayout(filaFeed);

    listaPublicaciones = new QListWidget();
    QPushButton* botonEliminar = new QPushButton("Eliminar publicacion seleccionada");
    pubEstado = new QLabel();

    layout->addWidget(grupoNueva);
    layout->addWidget(grupoFeed);
    layout->addWidget(listaPublicaciones);
    layout->addWidget(botonEliminar);
    layout->addWidget(pubEstado);

    connect(botonPublicar, &QPushButton::clicked, this, &MainWindow::crearPublicacion);
    connect(botonFeed, &QPushButton::clicked, this, &MainWindow::buscarFeedPorRango);
    connect(botonTodas, &QPushButton::clicked, this, &MainWindow::refrescarFeedCompleto);
    connect(botonEliminar, &QPushButton::clicked, this, &MainWindow::eliminarPublicacionSeleccionada);

    return pagina;
}

void MainWindow::crearPublicacion() {
    if (usuarioActualId == -1) return;
    if (pubContenido->text().isEmpty() || pubFecha->text().size() != 8) {
        pubEstado->setText("Completa el contenido y una fecha valida (AAAAMMDD)");
        pubEstado->setStyleSheet("color: red;");
        return;
    }

    Publicacion p;
    p.id = siguientePublicacionId++;
    p.idAutor = usuarioActualId;
    p.contenido = pubContenido->text().toStdString();
    p.fecha = pubFecha->text().toInt();
    arbol.insertar(p);

    pubContenido->clear();
    pubEstado->setText("Publicacion #" + QString::number(p.id) + " creada");
    pubEstado->setStyleSheet("color: green;");
    refrescarFeedCompleto();
    refrescarComboPublicaciones();
}

void MainWindow::refrescarFeedCompleto() {
    listaPublicaciones->clear();
    for (const Publicacion& p : arbol.obtenerTodas()) {
        QString autor = grafo.existeUsuario(p.idAutor) ? QString::fromStdString(grafo.obtenerUsuario(p.idAutor).nombre) : "?";
        QListWidgetItem* item = new QListWidgetItem(
            QString("#%1 [%2] %3: %4").arg(p.id).arg(QString::fromStdString(formatearFecha(p.fecha))).arg(autor).arg(QString::fromStdString(p.contenido)));
        item->setData(Qt::UserRole, p.id);
        listaPublicaciones->addItem(item);
    }
}

void MainWindow::buscarFeedPorRango() {
    if (feedDesde->text().size() != 8 || feedHasta->text().size() != 8) {
        pubEstado->setText("Las dos fechas deben tener formato AAAAMMDD");
        pubEstado->setStyleSheet("color: red;");
        return;
    }
    listaPublicaciones->clear();
    for (const Publicacion& p : arbol.obtenerFeed(feedDesde->text().toInt(), feedHasta->text().toInt())) {
        QString autor = grafo.existeUsuario(p.idAutor) ? QString::fromStdString(grafo.obtenerUsuario(p.idAutor).nombre) : "?";
        QListWidgetItem* item = new QListWidgetItem(
            QString("#%1 [%2] %3: %4").arg(p.id).arg(QString::fromStdString(formatearFecha(p.fecha))).arg(autor).arg(QString::fromStdString(p.contenido)));
        item->setData(Qt::UserRole, p.id);
        listaPublicaciones->addItem(item);
    }
}

void MainWindow::eliminarPublicacionSeleccionada() {
    QListWidgetItem* item = listaPublicaciones->currentItem();
    if (!item) return;
    arbol.eliminar(item->data(Qt::UserRole).toInt());
    refrescarFeedCompleto();
    refrescarComboPublicaciones();
}

// ==================== BUSQUEDA (Hashing Dinamico) ====================

QWidget* MainWindow::crearTabBusqueda() {
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    QHBoxLayout* filaBusqueda = new QHBoxLayout();
    buscarTexto = new QLineEdit();
    buscarTexto->setPlaceholderText("Texto a buscar...");
    buscarPor = new QComboBox();
    buscarPor->addItem("Nombre");
    buscarPor->addItem("Carrera");
    buscarPor->addItem("Institucion");
    QPushButton* botonBuscar = new QPushButton("Buscar");
    filaBusqueda->addWidget(buscarTexto);
    filaBusqueda->addWidget(buscarPor);
    filaBusqueda->addWidget(botonBuscar);

    resultadosBusqueda = new QListWidget();

    layout->addLayout(filaBusqueda);
    layout->addWidget(resultadosBusqueda);

    connect(botonBuscar, &QPushButton::clicked, this, &MainWindow::ejecutarBusqueda);
    return pagina;
}

void MainWindow::ejecutarBusqueda() {
    resultadosBusqueda->clear();
    string texto = buscarTexto->text().toStdString();
    vector<Usuario> resultados;

    if (buscarPor->currentIndex() == 0) {
        Usuario* u = hash.buscarPorNombre(texto);
        if (u) resultados.push_back(*u);
    }
    else if (buscarPor->currentIndex() == 1) {
        resultados = hash.buscarPorCarrera(texto);
    }
    else {
        resultados = hash.buscarPorInstitucion(texto);
    }

    if (resultados.empty()) {
        resultadosBusqueda->addItem("Sin resultados");
        return;
    }
    for (const Usuario& u : resultados) {
        resultadosBusqueda->addItem(QString::fromStdString(u.nombre) + " - " + QString::fromStdString(u.correo)
            + " - " + QString::fromStdString(u.carrera) + " - " + QString::fromStdString(u.institucion));
    }
}

// ==================== NOTIFICACIONES ====================

QWidget* MainWindow::crearTabNotificaciones() {
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    layout->addWidget(new QLabel("Estas notificaciones son de TODO el sistema (la cola no distingue destinatarios visualmente,\n"
        "pero cada Notificacion sabe a quien va dirigida en su campo idUsuarioDestino)."));
    notifCantidad = new QLabel();
    notifSiguiente = new QLabel();
    notifSiguiente->setWordWrap(true);
    QPushButton* botonLeida = new QPushButton("Marcar como leida (desencolar)");
    QPushButton* botonPrueba = new QPushButton("Generar notificacion de prueba");

    layout->addWidget(notifCantidad);
    layout->addWidget(new QLabel("Siguiente en la cola:"));
    layout->addWidget(notifSiguiente);
    layout->addWidget(botonLeida);
    layout->addWidget(botonPrueba);
    layout->addStretch();

    connect(botonLeida, &QPushButton::clicked, this, &MainWindow::marcarNotificacionLeida);
    connect(botonPrueba, &QPushButton::clicked, this, &MainWindow::generarNotificacionDePrueba);

    return pagina;
}

void MainWindow::refrescarNotificaciones() {
    notifCantidad->setText("Notificaciones pendientes: " + QString::number(colaNotif.tamano()));
    if (colaNotif.estaVacia()) {
        notifSiguiente->setText("(no hay notificaciones pendientes)");
    }
    else {
        Notificacion n = colaNotif.verSiguiente();
        notifSiguiente->setText(QString("Para usuario #%1: %2  [%3]").arg(n.idUsuarioDestino)
            .arg(QString::fromStdString(n.mensaje)).arg(QString::fromStdString(formatearFecha(n.fecha))));
    }
}

void MainWindow::marcarNotificacionLeida() {
    if (!colaNotif.estaVacia()) colaNotif.desencolar();
    refrescarNotificaciones();
}

void MainWindow::generarNotificacionDePrueba() {
    if (usuarioActualId == -1) return;
    notificar(usuarioActualId, "Notificacion de prueba generada manualmente", 0);
    refrescarNotificaciones();
}

// ==================== COMENTARIOS ====================

QWidget* MainWindow::crearTabComentarios() {
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    QHBoxLayout* filaSeleccion = new QHBoxLayout();
    filaSeleccion->addWidget(new QLabel("Publicacion:"));
    comentPublicacionId = new QComboBox();
    filaSeleccion->addWidget(comentPublicacionId);

    listaComentarios = new QListWidget();

    QHBoxLayout* filaNuevo = new QHBoxLayout();
    comentTexto = new QLineEdit();
    comentTexto->setPlaceholderText("Escribi un comentario...");
    QPushButton* botonComentar = new QPushButton("Comentar");
    filaNuevo->addWidget(comentTexto);
    filaNuevo->addWidget(botonComentar);

    layout->addLayout(filaSeleccion);
    layout->addWidget(listaComentarios);
    layout->addLayout(filaNuevo);

    connect(comentPublicacionId, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MainWindow::refrescarComentarios);
    connect(botonComentar, &QPushButton::clicked, this, &MainWindow::agregarComentario);

    return pagina;
}

void MainWindow::refrescarComboPublicaciones() {
    int idPrevio = comentPublicacionId->count() > 0 ? comentPublicacionId->currentData().toInt() : -1;
    comentPublicacionId->clear();
    int indiceARestaurar = -1, i = 0;
    for (const Publicacion& p : arbol.obtenerTodas()) {
        comentPublicacionId->addItem(QString("#%1 - %2").arg(p.id).arg(QString::fromStdString(p.contenido)), p.id);
        if (p.id == idPrevio) indiceARestaurar = i;
        i++;
    }
    if (indiceARestaurar != -1) comentPublicacionId->setCurrentIndex(indiceARestaurar);
}

void MainWindow::agregarComentario() {
    if (usuarioActualId == -1 || comentPublicacionId->count() == 0 || comentTexto->text().isEmpty()) return;

    int idPub = comentPublicacionId->currentData().toInt();
    QDate hoy = QDate::currentDate();
    int fecha = hoy.year() * 10000 + hoy.month() * 100 + hoy.day();
    gestorComentarios.agregarComentario(usuarioActualId, idPub, comentTexto->text().toStdString(), fecha);

    // Avisar al autor de la publicacion, si no es quien comento
    for (const Publicacion& p : arbol.obtenerTodas()) {
        if (p.id == idPub && p.idAutor != usuarioActualId) {
            notificar(p.idAutor, grafo.obtenerUsuario(usuarioActualId).nombre + " comento tu publicacion", 1);
            break;
        }
    }

    comentTexto->clear();
    refrescarComentarios();
}

void MainWindow::refrescarComentarios() {
    listaComentarios->clear();
    if (comentPublicacionId->count() == 0) return;
    int idPub = comentPublicacionId->currentData().toInt();
    for (const Comentario& c : gestorComentarios.obtenerPorPublicacion(idPub)) {
        QString autor = grafo.existeUsuario(c.idUsuario) ? QString::fromStdString(grafo.obtenerUsuario(c.idUsuario).nombre) : "?";
        listaComentarios->addItem("[" + QString::fromStdString(formatearFecha(c.fecha)) + "] " + autor + ": " + QString::fromStdString(c.contenido));
    }
}