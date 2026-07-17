#ifndef BPLUSTREE_CPP
#define BPLUSTREE_CPP

#include "bplustree.h"

template <typename T>
BPlusTree<T>::BPlusTree(int _grado) : grado(_grado), cantidadElementos(0) {
    if (grado < 2) grado = 2;
    raiz = make_shared<Nodo>(true);
}

template <typename T>
BPlusTree<T>::~BPlusTree() {
    // Los shared_ptr se encargan automáticamente
}

template <typename T>
void BPlusTree<T>::insertar(const T& clave) {
    if (raiz->claves.size() == grado - 1) {
        // La raíz está llena, crear nueva raíz
        auto nuevaRaiz = make_shared<Nodo>(false);
        nuevaRaiz->hijos.push_back(raiz);
        dividirNodo(raiz, nuevaRaiz, 0);
        raiz = nuevaRaiz;
    }

    insertarEnNodo(raiz, clave);
    cantidadElementos++;
}

template <typename T>
void BPlusTree<T>::insertarEnNodo(shared_ptr<Nodo>& nodo, const T& clave) {
    if (nodo->esHoja) {
        insertarEnHoja(nodo, clave);
        return;
    }

    // Nodo interno: buscar el hijo adecuado
    int i = 0;
    while (i < nodo->claves.size() && clave > nodo->claves[i]) {
        i++;
    }

    auto& hijo = nodo->hijos[i];

    // Verificar si el hijo está lleno
    if (hijo->claves.size() == grado - 1) {
        dividirNodo(hijo, nodo, i);
        // Después de dividir, decidir a qué hijo ir
        if (clave > nodo->claves[i]) {
            i++;
        }
    }

    insertarEnNodo(nodo->hijos[i], clave);
}

template <typename T>
void BPlusTree<T>::insertarEnHoja(shared_ptr<Nodo>& hoja, const T& clave) {
    auto pos = lower_bound(hoja->claves.begin(), hoja->claves.end(), clave);
    hoja->claves.insert(pos, clave);
}

template <typename T>
void BPlusTree<T>::dividirNodo(shared_ptr<Nodo>& nodo, shared_ptr<Nodo>& padre, int indice) {
    int mitad = (grado - 1) / 2;
    auto nuevoNodo = make_shared<Nodo>(nodo->esHoja);

    // Mover la segunda mitad de las claves al nuevo nodo
    nuevoNodo->claves.assign(nodo->claves.begin() + mitad, nodo->claves.end());
    nodo->claves.erase(nodo->claves.begin() + mitad, nodo->claves.end());

    if (nodo->esHoja) {
        // Para nodos hoja, mantener enlaces
        nuevoNodo->siguiente = nodo->siguiente;
        if (nodo->siguiente) {
            nodo->siguiente->anterior = nuevoNodo;
        }
        nodo->siguiente = nuevoNodo;
        nuevoNodo->anterior = nodo;

        // La clave de separación es la primera del nuevo nodo
        padre->claves.insert(padre->claves.begin() + indice, nuevoNodo->claves[0]);
    }
    else {
        // Para nodos internos
        T claveSeparacion = nodo->claves.back();
        nodo->claves.pop_back();

        // Mover los hijos correspondientes
        int mitadHijos = mitad + 1;
        nuevoNodo->hijos.assign(nodo->hijos.begin() + mitadHijos, nodo->hijos.end());
        nodo->hijos.erase(nodo->hijos.begin() + mitadHijos, nodo->hijos.end());

        padre->claves.insert(padre->claves.begin() + indice, claveSeparacion);
    }

    padre->hijos.insert(padre->hijos.begin() + indice + 1, nuevoNodo);
}

template <typename T>
bool BPlusTree<T>::buscar(const T& clave) const {
    auto hoja = buscarHoja(clave);
    return binary_search(hoja->claves.begin(), hoja->claves.end(), clave);
}

template <typename T>
shared_ptr<typename BPlusTree<T>::Nodo> BPlusTree<T>::buscarHoja(const T& clave) const {
    auto actual = raiz;
    while (!actual->esHoja) {
        int i = 0;
        while (i < actual->claves.size() && clave >= actual->claves[i]) {
            i++;
        }
        actual = actual->hijos[i];
    }
    return actual;
}

template <typename T>
vector<T> BPlusTree<T>::buscarRango(const T& inicio, const T& fin) const {
    vector<T> resultado;
    if (vacio()) return resultado;

    auto hoja = buscarHoja(inicio);
    recorrerRango(hoja, inicio, fin, resultado);
    return resultado;
}

template <typename T>
void BPlusTree<T>::recorrerRango(shared_ptr<Nodo> nodo, const T& inicio, const T& fin, vector<T>& resultado) const {
    if (!nodo) return;

    for (const auto& clave : nodo->claves) {
        if (clave >= inicio && clave <= fin) {
            resultado.push_back(clave);
        }
        if (clave > fin) break;
    }

    if (!nodo->claves.empty() && nodo->claves.back() < fin) {
        recorrerRango(nodo->siguiente, inicio, fin, resultado);
    }
}

template <typename T>
vector<T> BPlusTree<T>::obtenerTodos() const {
    vector<T> resultado;
    if (vacio()) return resultado;

    auto actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }

    recorrerHojas(actual, resultado);
    return resultado;
}

template <typename T>
void BPlusTree<T>::recorrerHojas(shared_ptr<Nodo> nodo, vector<T>& resultado) const {
    if (!nodo) return;

    for (const auto& clave : nodo->claves) {
        resultado.push_back(clave);
    }

    if (nodo->siguiente) {
        recorrerHojas(nodo->siguiente, resultado);
    }
}

template <typename T>
bool BPlusTree<T>::eliminar(const T& clave) {
    // Implementación simplificada
    // En una implementación completa, necesitarías rebalanceo
    auto hoja = buscarHoja(clave);
    auto it = find(hoja->claves.begin(), hoja->claves.end(), clave);
    if (it != hoja->claves.end()) {
        hoja->claves.erase(it);
        cantidadElementos--;
        return true;
    }
    return false;
}

template <typename T>
bool BPlusTree<T>::guardar(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Guardar metadatos
    archivo.write(reinterpret_cast<const char*>(&grado), sizeof(grado));
    archivo.write(reinterpret_cast<const char*>(&cantidadElementos), sizeof(cantidadElementos));

    // Guardar todas las hojas en orden
    auto actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }

    while (actual) {
        size_t tam = actual->claves.size();
        archivo.write(reinterpret_cast<const char*>(&tam), sizeof(tam));
        for (const auto& clave : actual->claves) {
            // Necesitas una función de serialización para T
            // Esto es un placeholder - deberías implementar serialización específica
            archivo.write(reinterpret_cast<const char*>(&clave), sizeof(clave));
        }
        actual = actual->siguiente;
    }

    archivo.close();
    return true;
}

template <typename T>
bool BPlusTree<T>::cargar(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Limpiar árbol actual
    raiz = make_shared<Nodo>(true);
    cantidadElementos = 0;

    // Leer metadatos
    int gradoLeido;
    archivo.read(reinterpret_cast<char*>(&gradoLeido), sizeof(gradoLeido));
    archivo.read(reinterpret_cast<char*>(&cantidadElementos), sizeof(cantidadElementos));

    if (gradoLeido != grado) {
        archivo.close();
        return false;
    }

    // Reconstruir hojas
    for (int i = 0; i < cantidadElementos; i++) {
        size_t tam;
        archivo.read(reinterpret_cast<char*>(&tam), sizeof(tam));
        for (size_t j = 0; j < tam; j++) {
            T clave;
            archivo.read(reinterpret_cast<char*>(&clave), sizeof(clave));
            insertar(clave);
        }
    }

    archivo.close();
    return true;
}

template <typename T>
void BPlusTree<T>::imprimir() const {
    cout << "=== Árbol B+ (Grado " << grado << ", " << cantidadElementos << " elementos) ===" << endl;
    auto actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }

    int contador = 0;
    while (actual) {
        for (const auto& clave : actual->claves) {
            cout << "[" << contador++ << "] " << clave.toString() << endl;
        }
        actual = actual->siguiente;
    }
    cout << "=====================================" << endl;
}

#endif