#include "redblacktree.h"
#include <fstream>
#include <sstream>
using namespace std;

NodoRB::NodoRB(int _id, const Usuario& _usuario, NodoRB* nil)
    : id(_id), usuario(_usuario), color(Color::ROJO),
    izquierdo(nil), derecho(nil), padre(nil) {
}

RedBlackTree::RedBlackTree() {
    // el NIL centinela es su propio "padre" al inicio, y siempre queda NEGRO.
    // Usuario{} son valores por defecto, nunca se leen porque NIL no representa
    // un usuario real, solo marca los bordes del árbol.
    NIL = new NodoRB(-1, Usuario{}, nullptr);
    NIL->color = Color::NEGRO;
    NIL->izquierdo = NIL;
    NIL->derecho = NIL;
    NIL->padre = NIL;
    raiz = NIL;
}

RedBlackTree::~RedBlackTree() {
    liberar(raiz);
    delete NIL;
}

void RedBlackTree::liberar(NodoRB* nodo) {
    if (nodo == NIL) return;
    liberar(nodo->izquierdo);
    liberar(nodo->derecho);
    delete nodo;
}

// ---------- Rotaciones ----------

void RedBlackTree::rotarIzquierda(NodoRB* x) {
    NodoRB* y = x->derecho;
    x->derecho = y->izquierdo;
    if (y->izquierdo != NIL) {
        y->izquierdo->padre = x;
    }
    y->padre = x->padre;
    if (x->padre == NIL) {
        raiz = y;
    }
    else if (x == x->padre->izquierdo) {
        x->padre->izquierdo = y;
    }
    else {
        x->padre->derecho = y;
    }
    y->izquierdo = x;
    x->padre = y;
}

void RedBlackTree::rotarDerecha(NodoRB* x) {
    NodoRB* y = x->izquierdo;
    x->izquierdo = y->derecho;
    if (y->derecho != NIL) {
        y->derecho->padre = x;
    }
    y->padre = x->padre;
    if (x->padre == NIL) {
        raiz = y;
    }
    else if (x == x->padre->derecho) {
        x->padre->derecho = y;
    }
    else {
        x->padre->izquierdo = y;
    }
    y->derecho = x;
    x->padre = y;
}

// ---------- Inserción ----------

bool RedBlackTree::conectar(const Usuario& usuario) {
    if (buscarNodo(usuario.id) != NIL) {
        return false; // ya estaba en línea, no se duplica
    }

    NodoRB* z = new NodoRB(usuario.id, usuario, NIL);

    NodoRB* y = NIL;
    NodoRB* x = raiz;
    while (x != NIL) {
        y = x;
        if (z->id < x->id) {
            x = x->izquierdo;
        }
        else {
            x = x->derecho;
        }
    }
    z->padre = y;
    if (y == NIL) {
        raiz = z;
    }
    else if (z->id < y->id) {
        y->izquierdo = z;
    }
    else {
        y->derecho = z;
    }
    // z ya nace ROJO (ver constructor de NodoRB) y sus hijos ya son NIL
    arreglarInsercion(z);
    return true;
}

void RedBlackTree::arreglarInsercion(NodoRB* z) {
    // mientras el padre sea ROJO hay una violación (dos rojos seguidos)
    while (z->padre->color == Color::ROJO) {
        if (z->padre == z->padre->padre->izquierdo) {
            NodoRB* tio = z->padre->padre->derecho;
            if (tio->color == Color::ROJO) {
                // Caso 1: el tío es rojo -> recolorear y subir el problema
                z->padre->color = Color::NEGRO;
                tio->color = Color::NEGRO;
                z->padre->padre->color = Color::ROJO;
                z = z->padre->padre;
            }
            else {
                if (z == z->padre->derecho) {
                    // Caso 2: zig-zag -> rotar para dejarlo en línea recta (caso 3)
                    z = z->padre;
                    rotarIzquierda(z);
                }
                // Caso 3: línea recta -> recolorear y rotar
                z->padre->color = Color::NEGRO;
                z->padre->padre->color = Color::ROJO;
                rotarDerecha(z->padre->padre);
            }
        }
        else {
            // mismo análisis pero espejado (padre es hijo derecho del abuelo)
            NodoRB* tio = z->padre->padre->izquierdo;
            if (tio->color == Color::ROJO) {
                z->padre->color = Color::NEGRO;
                tio->color = Color::NEGRO;
                z->padre->padre->color = Color::ROJO;
                z = z->padre->padre;
            }
            else {
                if (z == z->padre->izquierdo) {
                    z = z->padre;
                    rotarDerecha(z);
                }
                z->padre->color = Color::NEGRO;
                z->padre->padre->color = Color::ROJO;
                rotarIzquierda(z->padre->padre);
            }
        }
    }
    raiz->color = Color::NEGRO; // la raíz siempre queda negra
}

// ---------- Búsqueda ----------

NodoRB* RedBlackTree::buscarNodo(int id) const {
    NodoRB* actual = raiz;
    while (actual != NIL && actual->id != id) {
        if (id < actual->id) {
            actual = actual->izquierdo;
        }
        else {
            actual = actual->derecho;
        }
    }
    return actual;
}

bool RedBlackTree::estaEnLinea(int idUsuario) const {
    return buscarNodo(idUsuario) != NIL;
}

NodoRB* RedBlackTree::minimo(NodoRB* nodo) const {
    while (nodo->izquierdo != NIL) {
        nodo = nodo->izquierdo;
    }
    return nodo;
}

// ---------- Eliminación ----------
// trasplantar reemplaza el subárbol en u por el subárbol en v,
// solo ajustando el lado del padre de u. No toca los hijos de v.
void RedBlackTree::trasplantar(NodoRB* u, NodoRB* v) {
    if (u->padre == NIL) {
        raiz = v;
    }
    else if (u == u->padre->izquierdo) {
        u->padre->izquierdo = v;
    }
    else {
        u->padre->derecho = v;
    }
    v->padre = u->padre;
}

bool RedBlackTree::desconectar(int idUsuario) {
    NodoRB* z = buscarNodo(idUsuario);
    if (z == NIL) {
        return false; // no estaba en línea
    }

    NodoRB* y = z;
    Color colorOriginalDeY = y->color;
    NodoRB* x;

    if (z->izquierdo == NIL) {
        x = z->derecho;
        trasplantar(z, z->derecho);
    }
    else if (z->derecho == NIL) {
        x = z->izquierdo;
        trasplantar(z, z->izquierdo);
    }
    else {
        // z tiene dos hijos: se reemplaza por su sucesor (el mínimo del subárbol derecho)
        y = minimo(z->derecho);
        colorOriginalDeY = y->color;
        x = y->derecho;

        if (y->padre == z) {
            x->padre = y; // por si x es NIL, para que arreglarEliminacion pueda subir desde ahí
        }
        else {
            trasplantar(y, y->derecho);
            y->derecho = z->derecho;
            y->derecho->padre = y;
        }
        trasplantar(z, y);
        y->izquierdo = z->izquierdo;
        y->izquierdo->padre = y;
        y->color = z->color;
    }

    delete z;

    // si se quitó un nodo negro, se rompió la propiedad de "misma cantidad
    // de negros en todo camino a una hoja" -> hay que rebalancear
    if (colorOriginalDeY == Color::NEGRO) {
        arreglarEliminacion(x);
    }
    return true;
}

void RedBlackTree::arreglarEliminacion(NodoRB* x) {
    // x representa un "doble negro": tiene un negro de más que le sobra por acomodar
    while (x != raiz && x->color == Color::NEGRO) {
        if (x == x->padre->izquierdo) {
            NodoRB* hermano = x->padre->derecho;
            if (hermano->color == Color::ROJO) {
                // Caso 1: hermano rojo -> rotar para convertirlo en uno de los casos negros
                hermano->color = Color::NEGRO;
                x->padre->color = Color::ROJO;
                rotarIzquierda(x->padre);
                hermano = x->padre->derecho;
            }
            if (hermano->izquierdo->color == Color::NEGRO && hermano->derecho->color == Color::NEGRO) {
                // Caso 2: ambos sobrinos negros -> recolorear hermano y subir el doble negro
                hermano->color = Color::ROJO;
                x = x->padre;
            }
            else {
                if (hermano->derecho->color == Color::NEGRO) {
                    // Caso 3: sobrino derecho negro -> rotar para dejarlo como caso 4
                    hermano->izquierdo->color = Color::NEGRO;
                    hermano->color = Color::ROJO;
                    rotarDerecha(hermano);
                    hermano = x->padre->derecho;
                }
                // Caso 4: sobrino derecho rojo -> recolorear y rotar, termina el ciclo
                hermano->color = x->padre->color;
                x->padre->color = Color::NEGRO;
                hermano->derecho->color = Color::NEGRO;
                rotarIzquierda(x->padre);
                x = raiz;
            }
        }
        else {
            // espejo del bloque anterior (x es hijo derecho)
            NodoRB* hermano = x->padre->izquierdo;
            if (hermano->color == Color::ROJO) {
                hermano->color = Color::NEGRO;
                x->padre->color = Color::ROJO;
                rotarDerecha(x->padre);
                hermano = x->padre->izquierdo;
            }
            if (hermano->derecho->color == Color::NEGRO && hermano->izquierdo->color == Color::NEGRO) {
                hermano->color = Color::ROJO;
                x = x->padre;
            }
            else {
                if (hermano->izquierdo->color == Color::NEGRO) {
                    hermano->derecho->color = Color::NEGRO;
                    hermano->color = Color::ROJO;
                    rotarIzquierda(hermano);
                    hermano = x->padre->izquierdo;
                }
                hermano->color = x->padre->color;
                x->padre->color = Color::NEGRO;
                hermano->izquierdo->color = Color::NEGRO;
                rotarDerecha(x->padre);
                x = raiz;
            }
        }
    }
    x->color = Color::NEGRO;
}

// ---------- Recorridos ----------

void RedBlackTree::inOrden(NodoRB* nodo, vector<Usuario>& resultado) const {
    if (nodo == NIL) return;
    inOrden(nodo->izquierdo, resultado);
    resultado.push_back(nodo->usuario);
    inOrden(nodo->derecho, resultado);
}

vector<Usuario> RedBlackTree::listarEnLinea() const {
    vector<Usuario> resultado;
    inOrden(raiz, resultado);
    return resultado;
}

int RedBlackTree::cantidadEnLinea() const {
    return (int)listarEnLinea().size(); // simple; si el rendimiento importa se puede
                                        // llevar un contador aparte como en NotificationQueue
}

// ---------- Persistencia ----------
// Mismo formato "|" que el resto de las estructuras del proyecto.
// Al cargar, se reinserta con conectar() en vez de reconstruir punteros a mano,
// igual que hacen bplustree y notificationqueue.

void RedBlackTree::guardarEnArchivo(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo);
    vector<Usuario> usuarios = listarEnLinea();
    for (const Usuario& u : usuarios) {
        archivo << u.id << "|" << u.nombre << "|" << u.correo << "|"
            << u.carrera << "|" << u.institucion << "|"
            << (int)u.tipo << "|" << u.contrasena << endl;
    }
    archivo.close();
}

void RedBlackTree::cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return;

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string campo;
        Usuario u;

        getline(ss, campo, '|'); u.id = stoi(campo);
        getline(ss, campo, '|'); u.nombre = campo;
        getline(ss, campo, '|'); u.correo = campo;
        getline(ss, campo, '|'); u.carrera = campo;
        getline(ss, campo, '|'); u.institucion = campo;
        getline(ss, campo, '|'); u.tipo = (TipoUsuario)stoi(campo);
        getline(ss, campo, '|'); u.contrasena = campo;

        conectar(u);
    }
    archivo.close();
}
