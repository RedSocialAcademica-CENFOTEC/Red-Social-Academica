#include "redblacktree.h"

NodoRB::NodoRB(const Usuario& u, NodoRB* n)
    : dato(u), color(ColorNodo::ROJO), izquierdo(n), derecho(n), padre(n) {}

ArbolRojoNegro::ArbolRojoNegro() {
    nil = new NodoRB(Usuario{}, nullptr);
    nil->color = ColorNodo::NEGRO;
    nil->izquierdo = nil->derecho = nil->padre = nil;
    raiz = nil;
}

void ArbolRojoNegro::liberar(NodoRB* x) {
    if (x == nil) return;
    liberar(x->izquierdo);
    liberar(x->derecho);
    delete x;
}

ArbolRojoNegro::~ArbolRojoNegro() {
    liberar(raiz);
    delete nil;
}

// ==================== Rotaciones ====================

void ArbolRojoNegro::rotarIzquierda(NodoRB* x) {
    NodoRB* y = x->derecho;
    x->derecho = y->izquierdo;
    if (y->izquierdo != nil) y->izquierdo->padre = x;
    y->padre = x->padre;
    if (x->padre == nil) raiz = y;
    else if (x == x->padre->izquierdo) x->padre->izquierdo = y;
    else x->padre->derecho = y;
    y->izquierdo = x;
    x->padre = y;
}

void ArbolRojoNegro::rotarDerecha(NodoRB* x) {
    NodoRB* y = x->izquierdo;
    x->izquierdo = y->derecho;
    if (y->derecho != nil) y->derecho->padre = x;
    y->padre = x->padre;
    if (x->padre == nil) raiz = y;
    else if (x == x->padre->derecho) x->padre->derecho = y;
    else x->padre->izquierdo = y;
    y->derecho = x;
    x->padre = y;
}

// ==================== Insercion ====================

void ArbolRojoNegro::insertarFixup(NodoRB* z) {
    while (z->padre->color == ColorNodo::ROJO) {
        if (z->padre == z->padre->padre->izquierdo) {
            NodoRB* tio = z->padre->padre->derecho;
            if (tio->color == ColorNodo::ROJO) {
                z->padre->color = ColorNodo::NEGRO;
                tio->color = ColorNodo::NEGRO;
                z->padre->padre->color = ColorNodo::ROJO;
                z = z->padre->padre;
            } else {
                if (z == z->padre->derecho) {
                    z = z->padre;
                    rotarIzquierda(z);
                }
                z->padre->color = ColorNodo::NEGRO;
                z->padre->padre->color = ColorNodo::ROJO;
                rotarDerecha(z->padre->padre);
            }
        } else {
            NodoRB* tio = z->padre->padre->izquierdo;
            if (tio->color == ColorNodo::ROJO) {
                z->padre->color = ColorNodo::NEGRO;
                tio->color = ColorNodo::NEGRO;
                z->padre->padre->color = ColorNodo::ROJO;
                z = z->padre->padre;
            } else {
                if (z == z->padre->izquierdo) {
                    z = z->padre;
                    rotarDerecha(z);
                }
                z->padre->color = ColorNodo::NEGRO;
                z->padre->padre->color = ColorNodo::ROJO;
                rotarIzquierda(z->padre->padre);
            }
        }
    }
    raiz->color = ColorNodo::NEGRO;
}

void ArbolRojoNegro::insertar(const Usuario& usuario) {
    NodoRB* existente = buscarNodo(usuario.id);
    if (existente != nil) {
        existente->dato = usuario; // ya estaba en linea, solo actualizamos el dato
        return;
    }

    NodoRB* z = new NodoRB(usuario, nil);
    NodoRB* y = nil;
    NodoRB* x = raiz;

    while (x != nil) {
        y = x;
        if (z->dato.id < x->dato.id) x = x->izquierdo;
        else x = x->derecho;
    }
    z->padre = y;
    if (y == nil) raiz = z;
    else if (z->dato.id < y->dato.id) y->izquierdo = z;
    else y->derecho = z;

    z->izquierdo = nil;
    z->derecho = nil;
    z->color = ColorNodo::ROJO;
    insertarFixup(z);
}

// ==================== Busqueda ====================

NodoRB* ArbolRojoNegro::buscarNodo(int id) const {
    NodoRB* x = raiz;
    while (x != nil && id != x->dato.id) {
        if (id < x->dato.id) x = x->izquierdo;
        else x = x->derecho;
    }
    return x;
}

bool ArbolRojoNegro::estaEnLinea(int id) const {
    return buscarNodo(id) != nil;
}

Usuario ArbolRojoNegro::obtener(int id) const {
    NodoRB* n = buscarNodo(id);
    return n->dato; // si no existe, dato del centinela (Usuario{}); llamar con estaEnLinea antes
}

NodoRB* ArbolRojoNegro::minimo(NodoRB* x) const {
    while (x->izquierdo != nil) x = x->izquierdo;
    return x;
}

// ==================== Eliminacion ====================

void ArbolRojoNegro::transplantar(NodoRB* u, NodoRB* v) {
    if (u->padre == nil) raiz = v;
    else if (u == u->padre->izquierdo) u->padre->izquierdo = v;
    else u->padre->derecho = v;
    v->padre = u->padre;
}

void ArbolRojoNegro::eliminarFixup(NodoRB* x) {
    while (x != raiz && x->color == ColorNodo::NEGRO) {
        if (x == x->padre->izquierdo) {
            NodoRB* w = x->padre->derecho;
            if (w->color == ColorNodo::ROJO) {
                w->color = ColorNodo::NEGRO;
                x->padre->color = ColorNodo::ROJO;
                rotarIzquierda(x->padre);
                w = x->padre->derecho;
            }
            if (w->izquierdo->color == ColorNodo::NEGRO && w->derecho->color == ColorNodo::NEGRO) {
                w->color = ColorNodo::ROJO;
                x = x->padre;
            } else {
                if (w->derecho->color == ColorNodo::NEGRO) {
                    w->izquierdo->color = ColorNodo::NEGRO;
                    w->color = ColorNodo::ROJO;
                    rotarDerecha(w);
                    w = x->padre->derecho;
                }
                w->color = x->padre->color;
                x->padre->color = ColorNodo::NEGRO;
                w->derecho->color = ColorNodo::NEGRO;
                rotarIzquierda(x->padre);
                x = raiz;
            }
        } else {
            NodoRB* w = x->padre->izquierdo;
            if (w->color == ColorNodo::ROJO) {
                w->color = ColorNodo::NEGRO;
                x->padre->color = ColorNodo::ROJO;
                rotarDerecha(x->padre);
                w = x->padre->izquierdo;
            }
            if (w->derecho->color == ColorNodo::NEGRO && w->izquierdo->color == ColorNodo::NEGRO) {
                w->color = ColorNodo::ROJO;
                x = x->padre;
            } else {
                if (w->izquierdo->color == ColorNodo::NEGRO) {
                    w->derecho->color = ColorNodo::NEGRO;
                    w->color = ColorNodo::ROJO;
                    rotarIzquierda(w);
                    w = x->padre->izquierdo;
                }
                w->color = x->padre->color;
                x->padre->color = ColorNodo::NEGRO;
                w->izquierdo->color = ColorNodo::NEGRO;
                rotarDerecha(x->padre);
                x = raiz;
            }
        }
    }
    x->color = ColorNodo::NEGRO;
}

bool ArbolRojoNegro::eliminar(int id) {
    NodoRB* z = buscarNodo(id);
    if (z == nil) return false;

    NodoRB* y = z;
    NodoRB* x;
    ColorNodo colorOriginal = y->color;

    if (z->izquierdo == nil) {
        x = z->derecho;
        transplantar(z, z->derecho);
    } else if (z->derecho == nil) {
        x = z->izquierdo;
        transplantar(z, z->izquierdo);
    } else {
        y = minimo(z->derecho);
        colorOriginal = y->color;
        x = y->derecho;
        if (y->padre == z) {
            x->padre = y;
        } else {
            transplantar(y, y->derecho);
            y->derecho = z->derecho;
            y->derecho->padre = y;
        }
        transplantar(z, y);
        y->izquierdo = z->izquierdo;
        y->izquierdo->padre = y;
        y->color = z->color;
    }

    delete z;
    if (colorOriginal == ColorNodo::NEGRO) eliminarFixup(x);
    return true;
}

// ==================== Recorridos ====================

void ArbolRojoNegro::recorridoInorden(NodoRB* x, vector<Usuario>& resultado) const {
    if (x == nil) return;
    recorridoInorden(x->izquierdo, resultado);
    resultado.push_back(x->dato);
    recorridoInorden(x->derecho, resultado);
}

vector<Usuario> ArbolRojoNegro::obtenerTodosEnLinea() const {
    vector<Usuario> resultado;
    recorridoInorden(raiz, resultado);
    return resultado;
}

int ArbolRojoNegro::cantidadEnLinea() const {
    return (int)obtenerTodosEnLinea().size();
}
