#ifndef ROJONEGRO_CPP
#define ROJONEGRO_CPP

#include "rojonegro.h"
#include <stack>

template <typename K, typename V>
RedBlackTree<K, V>::RedBlackTree() : raiz(nullptr), cantidadElementos(0) {}

template <typename K, typename V>
RedBlackTree<K, V>::~RedBlackTree() {
    // Los shared_ptr se encargan automáticamente
}

template <typename K, typename V>
void RedBlackTree<K, V>::rotarIzquierda(shared_ptr<Nodo>& nodo) {
    auto hijoDerecho = nodo->derecho;
    nodo->derecho = hijoDerecho->izquierdo;

    if (hijoDerecho->izquierdo) {
        hijoDerecho->izquierdo->padre = nodo;
    }

    hijoDerecho->padre = nodo->padre;

    if (!nodo->padre) {
        raiz = hijoDerecho;
    }
    else if (nodo == nodo->padre->izquierdo) {
        nodo->padre->izquierdo = hijoDerecho;
    }
    else {
        nodo->padre->derecho = hijoDerecho;
    }

    hijoDerecho->izquierdo = nodo;
    nodo->padre = hijoDerecho;
}

template <typename K, typename V>
void RedBlackTree<K, V>::rotarDerecha(shared_ptr<Nodo>& nodo) {
    auto hijoIzquierdo = nodo->izquierdo;
    nodo->izquierdo = hijoIzquierdo->derecho;

    if (hijoIzquierdo->derecho) {
        hijoIzquierdo->derecho->padre = nodo;
    }

    hijoIzquierdo->padre = nodo->padre;

    if (!nodo->padre) {
        raiz = hijoIzquierdo;
    }
    else if (nodo == nodo->padre->izquierdo) {
        nodo->padre->izquierdo = hijoIzquierdo;
    }
    else {
        nodo->padre->derecho = hijoIzquierdo;
    }

    hijoIzquierdo->derecho = nodo;
    nodo->padre = hijoIzquierdo;
}

template <typename K, typename V>
void RedBlackTree<K, V>::balancearInsercion(shared_ptr<Nodo>& nodo) {
    while (nodo != raiz && nodo->padre->color == ROJO) {
        auto padre = nodo->padre;
        auto abuelo = padre->padre;

        if (padre == abuelo->izquierdo) {
            auto tio = abuelo->derecho;

            // Caso 1: El tío es rojo
            if (tio && tio->color == ROJO) {
                padre->color = NEGRO;
                tio->color = NEGRO;
                abuelo->color = ROJO;
                nodo = abuelo;
                continue;
            }

            // Caso 2: El tío es negro y nodo es hijo derecho
            if (nodo == padre->derecho) {
                rotarIzquierda(padre);
                nodo = padre;
                padre = nodo->padre;
            }

            // Caso 3: El tío es negro y nodo es hijo izquierdo
            padre->color = NEGRO;
            abuelo->color = ROJO;
            rotarDerecha(abuelo);
        }
        else {
            auto tio = abuelo->izquierdo;

            if (tio && tio->color == ROJO) {
                padre->color = NEGRO;
                tio->color = NEGRO;
                abuelo->color = ROJO;
                nodo = abuelo;
                continue;
            }

            if (nodo == padre->izquierdo) {
                rotarDerecha(padre);
                nodo = padre;
                padre = nodo->padre;
            }

            padre->color = NEGRO;
            abuelo->color = ROJO;
            rotarIzquierda(abuelo);
        }
    }

    raiz->color = NEGRO;
}

template <typename K, typename V>
void RedBlackTree<K, V>::insertar(const K& clave, const V& valor) {
    if (!raiz) {
        raiz = make_shared<Nodo>(clave, valor);
        raiz->color = NEGRO;
        cantidadElementos++;
        return;
    }

    insertar(raiz, nullptr, clave, valor);
}

template <typename K, typename V>
void RedBlackTree<K, V>::insertar(shared_ptr<Nodo>& nodo, shared_ptr<Nodo>& padre, const K& clave, const V& valor) {
    if (!nodo) {
        nodo = make_shared<Nodo>(clave, valor);
        nodo->padre = padre;
        cantidadElementos++;
        balancearInsercion(nodo);
        return;
    }

    if (clave < nodo->clave) {
        insertar(nodo->izquierdo, nodo, clave, valor);
    }
    else if (clave > nodo->clave) {
        insertar(nodo->derecho, nodo, clave, valor);
    }
    else {
        // Clave ya existe, actualizar valor
        nodo->valor = valor;
    }
}

template <typename K, typename V>
bool RedBlackTree<K, V>::buscar(const K& clave, V& valor) const {
    auto nodo = buscar(raiz, clave);
    if (nodo) {
        valor = nodo->valor;
        return true;
    }
    return false;
}

template <typename K, typename V>
shared_ptr<typename RedBlackTree<K, V>::Nodo>
RedBlackTree<K, V>::buscar(shared_ptr<Nodo> nodo, const K& clave) const {
    if (!nodo) return nullptr;
    if (clave == nodo->clave) return nodo;
    if (clave < nodo->clave) return buscar(nodo->izquierdo, clave);
    return buscar(nodo->derecho, clave);
}

template <typename K, typename V>
shared_ptr<typename RedBlackTree<K, V>::Nodo>
RedBlackTree<K, V>::minimo(shared_ptr<Nodo> nodo) const {
    if (!nodo) return nullptr;
    if (!nodo->izquierdo) return nodo;
    return minimo(nodo->izquierdo);
}

template <typename K, typename V>
shared_ptr<typename RedBlackTree<K, V>::Nodo>
RedBlackTree<K, V>::maximo(shared_ptr<Nodo> nodo) const {
    if (!nodo) return nullptr;
    if (!nodo->derecho) return nodo;
    return maximo(nodo->derecho);
}

template <typename K, typename V>
void RedBlackTree<K, V>::eliminar(shared_ptr<Nodo>& nodo, const K& clave) {
    if (!nodo) return;

    if (clave < nodo->clave) {
        eliminar(nodo->izquierdo, clave);
    }
    else if (clave > nodo->clave) {
        eliminar(nodo->derecho, clave);
    }
    else {
        // Nodo encontrado
        if (!nodo->izquierdo || !nodo->derecho) {
            // Nodo con un solo hijo o sin hijos
            auto hijo = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;

            if (!hijo) {
                // Nodo hoja
                if (nodo == raiz) {
                    raiz = nullptr;
                }
                else {
                    bool esIzquierdo = (nodo == nodo->padre->izquierdo);
                    nodo->padre->izquierdo = nullptr;
                    nodo->padre->derecho = nullptr;
                    // Balancear si es necesario
                }
                cantidadElementos--;
            }
            else {
                // Un solo hijo
                nodo->clave = hijo->clave;
                nodo->valor = hijo->valor;
                nodo->izquierdo = nullptr;
                nodo->derecho = nullptr;
                cantidadElementos--;
            }
        }
        else {
            // Dos hijos: reemplazar con el mínimo del subárbol derecho
            auto sucesor = minimo(nodo->derecho);
            nodo->clave = sucesor->clave;
            nodo->valor = sucesor->valor;
            eliminar(nodo->derecho, sucesor->clave);
        }
    }
}

template <typename K, typename V>
bool RedBlackTree<K, V>::eliminar(const K& clave) {
    if (!buscar(raiz, clave)) return false;
    eliminar(raiz, clave);
    if (raiz) raiz->color = NEGRO;
    return true;
}

template <typename K, typename V>
vector<pair<K, V>> RedBlackTree<K, V>::obtenerTodos() const {
    vector<pair<K, V>> resultado;
    recorridoInorden(raiz, resultado);
    return resultado;
}

template <typename K, typename V>
void RedBlackTree<K, V>::recorridoInorden(shared_ptr<Nodo> nodo, vector<pair<K, V>>& resultado) const {
    if (!nodo) return;
    recorridoInorden(nodo->izquierdo, resultado);
    resultado.push_back({ nodo->clave, nodo->valor });
    recorridoInorden(nodo->derecho, resultado);
}

template <typename K, typename V>
bool RedBlackTree<K, V>::guardar(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    archivo.write(reinterpret_cast<const char*>(&cantidadElementos), sizeof(cantidadElementos));
    serializar(raiz, archivo);
    archivo.close();
    return true;
}

template <typename K, typename V>
void RedBlackTree<K, V>::serializar(shared_ptr<Nodo> nodo, ofstream& archivo) const {
    if (!nodo) return;

    // Guardar información del nodo
    archivo.write(reinterpret_cast<const char*>(&nodo->clave), sizeof(nodo->clave));
    archivo.write(reinterpret_cast<const char*>(&nodo->valor), sizeof(nodo->valor));
    archivo.write(reinterpret_cast<const char*>(&nodo->color), sizeof(nodo->color));

    bool tieneIzq = nodo->izquierdo != nullptr;
    bool tieneDer = nodo->derecho != nullptr;
    archivo.write(reinterpret_cast<const char*>(&tieneIzq), sizeof(tieneIzq));
    archivo.write(reinterpret_cast<const char*>(&tieneDer), sizeof(tieneDer));

    if (tieneIzq) serializar(nodo->izquierdo, archivo);
    if (tieneDer) serializar(nodo->derecho, archivo);
}

template <typename K, typename V>
bool RedBlackTree<K, V>::cargar(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Limpiar árbol
    raiz = nullptr;
    cantidadElementos = 0;

    archivo.read(reinterpret_cast<char*>(&cantidadElementos), sizeof(cantidadElementos));
    // Implementación simplificada - en producción necesitarías reconstruir el árbol
    // balanceado correctamente

    archivo.close();
    return true;
}

template <typename K, typename V>
void RedBlackTree<K, V>::imprimir() const {
    cout << "=== ÁRBOL ROJO-NEGRO ===" << endl;
    cout << "Elementos: " << cantidadElementos << endl;
    auto elementos = obtenerTodos();
    for (const auto& par : elementos) {
        cout << "Clave: " << par.first << " -> Valor: " << par.second << endl;
    }
    cout << "=========================" << endl;
}

#endif