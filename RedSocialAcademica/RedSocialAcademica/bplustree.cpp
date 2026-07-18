// Arbol B+ para publicaciones ordenadas por fecha

#include "bplustree.h"
#include <iostream>
#include <fstream>
#include <sstream>

BPlusTree::BPlusTree(int _orden) : orden(_orden) {
    raiz = new NodoB(true); // el arbol arranca como una sola hoja vacia
}

NodoB::NodoB(bool hoja) : esHoja(hoja), siguiente(nullptr), padre(nullptr) {}

void BPlusTree::dividirHoja(NodoB* hoja) {
    int mid = (int)hoja->claves.size() / 2;
    NodoB* nuevaHoja = new NodoB(true);

    // La mitad derecha se va al nuevo nodo. Las hojas conservan TODAS las claves originales,
    // nada se pierde ni se borra, solo se reparten entre las dos hojas.
    for (int i = mid; i < (int)hoja->claves.size(); i++) {
        nuevaHoja->claves.push_back(hoja->claves[i]);
        nuevaHoja->publicaciones.push_back(hoja->publicaciones[i]);
    }
    hoja->claves.resize(mid);
    hoja->publicaciones.resize(mid);

    // Enlazamos las hojas para que el recorrido del feed siga funcionando
    nuevaHoja->siguiente = hoja->siguiente;
    hoja->siguiente = nuevaHoja;

    int claveAscendida = nuevaHoja->claves[0]; // OJO: es una copia, no se borra de la hoja

    if (hoja->padre == nullptr) {
        // La hoja era la raiz -> creamos una raiz nueva, interna, con dos hijos
        NodoB* nuevaRaiz = new NodoB(false);
        nuevaRaiz->claves.push_back(claveAscendida);
        nuevaRaiz->hijos.push_back(hoja);
        nuevaRaiz->hijos.push_back(nuevaHoja);
        hoja->padre = nuevaRaiz;
        nuevaHoja->padre = nuevaRaiz;
        raiz = nuevaRaiz;
    }
    else {
        nuevaHoja->padre = hoja->padre;
        insertarEnPadre(hoja->padre, claveAscendida, nuevaHoja);
    }
}

void BPlusTree::insertarEnPadre(NodoB* padre, int clave, NodoB* nuevoHijo) {
    // Buscamos donde insertar la clave nueva, manteniendo el orden
    int pos = 0;
    while (pos < (int)padre->claves.size() && padre->claves[pos] < clave) {
        pos++;
    }
    padre->claves.insert(padre->claves.begin() + pos, clave);
    padre->hijos.insert(padre->hijos.begin() + pos + 1, nuevoHijo);

    // Si el padre tambien se paso del orden, hay que dividirlo tambien (division interna)
    if ((int)padre->claves.size() > orden) {
        int mid = (int)padre->claves.size() / 2;
        int claveAscendida = padre->claves[mid]; // aca SI se saca del nodo, es un nodo interno

        NodoB* nuevoInterno = new NodoB(false);
        for (int i = mid + 1; i < (int)padre->claves.size(); i++) {
            nuevoInterno->claves.push_back(padre->claves[i]);
        }
        for (int i = mid + 1; i < (int)padre->hijos.size(); i++) {
            nuevoInterno->hijos.push_back(padre->hijos[i]);
            padre->hijos[i]->padre = nuevoInterno;
        }
        padre->claves.resize(mid);
        padre->hijos.resize(mid + 1);

        if (padre->padre == nullptr) {
            NodoB* nuevaRaiz = new NodoB(false);
            nuevaRaiz->claves.push_back(claveAscendida);
            nuevaRaiz->hijos.push_back(padre);
            nuevaRaiz->hijos.push_back(nuevoInterno);
            padre->padre = nuevaRaiz;
            nuevoInterno->padre = nuevaRaiz;
            raiz = nuevaRaiz;
        }
        else {
            nuevoInterno->padre = padre->padre;
            insertarEnPadre(padre->padre, claveAscendida, nuevoInterno);
        }
    }
}

NodoB* BPlusTree::buscarHoja(int clave) {
    NodoB* actual = raiz;
    while (!actual->esHoja) {
        int i = 0;
        while (i < (int)actual->claves.size() && clave >= actual->claves[i]) {
            i++;
        }
        actual = actual->hijos[i];
    }
    return actual;
}

void BPlusTree::insertar(const Publicacion& pub) {
    NodoB* hoja = buscarHoja(pub.fecha);

    // Buscamos la posicion donde debe ir, para mantener el vector ordenado por fecha
    int pos = 0;
    while (pos < (int)hoja->claves.size() && hoja->claves[pos] < pub.fecha) {
        pos++;
    }

    //Los dos insert() en paralelo: como claves y publicaciones son vectores separados pero relacionados por posición
    hoja->claves.insert(hoja->claves.begin() + pos, pub.fecha);
    hoja->publicaciones.insert(hoja->publicaciones.begin() + pos, pub);

    // Por ahora cubrimos solo el caso simple: la hoja tenia espacio.
    // El caso en que se llena y hay que dividirla lo agregamos en el siguiente paso.
    if ((int)hoja->claves.size() > orden) {
        dividirHoja(hoja);
    }
}

// usamos buscarHoja(fechaInicio) para caer directo en la hoja correcta, Después vamos avanzando hoja por hoja con siguiente, revisando cada clave: si es menor a fechaInicio
vector<Publicacion> BPlusTree::obtenerFeed(int fechaInicio, int fechaFin) {
    vector<Publicacion> resultado;

    NodoB* actual = buscarHoja(fechaInicio); // arrancamos en la hoja donde deberia estar fechaInicio

    bool terminado = false;
    while (actual != nullptr && !terminado) {
        for (size_t i = 0; i < actual->claves.size(); i++) {
            if (actual->claves[i] > fechaFin) {
                terminado = true; // ya nos pasamos del rango, no seguimos
                break;
            }
            if (actual->claves[i] >= fechaInicio) {
                resultado.push_back(actual->publicaciones[i]);
            }
        }
        actual = actual->siguiente; // avanzamos a la siguiente hoja enlazada
    }

    return resultado;
}

/*Cuando se divide una hoja, la clave que sube al padre es solo una copia — la hoja se queda con ella también. Pero cuando se divide un nodo interno
la clave del medio sí se saca por completo del nodo y solo queda arriba, en el padre — eso es lo que diferencia un Árbol B+ de un Árbol B normal
y es la diferencia entre claveAscendida = nuevaHoja->claves[0] (se queda) vs claveAscendida = padre->claves[mid] (se va).
*/

void BPlusTree::imprimirHojas() const {
    NodoB* actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0]; // bajamos siempre por el hijo mas a la izquierda
    }

    cout << "Recorrido de hojas (fechas ordenadas):" << endl;
    while (actual != nullptr) {
        cout << "  Hoja: [";
        for (size_t i = 0; i < actual->claves.size(); i++) {
            cout << actual->claves[i];
            if (i + 1 < actual->claves.size()) cout << ", ";
        }
        cout << "]" << endl;
        actual = actual->siguiente;
    }
}

/*Para eliminar, el árbol está organizado por fecha, no por id,  para eliminar por id no hay atajo — hay que recorrer las hojas una por una buscando 
la publicación.*/

bool BPlusTree::eliminar(int idPublicacion) {
    NodoB* actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0]; // arrancamos en la hoja mas a la izquierda
    }

    while (actual != nullptr) {
        for (size_t i = 0; i < actual->publicaciones.size(); i++) {
            if (actual->publicaciones[i].id == idPublicacion) {
                actual->claves.erase(actual->claves.begin() + i);
                actual->publicaciones.erase(actual->publicaciones.begin() + i);
                return true; // encontrada y eliminada
            }
        }
        actual = actual->siguiente;
    }

    return false; // no se encontro ninguna publicacion con ese id
}

vector<Publicacion> BPlusTree::obtenerTodas() {
    vector<Publicacion> resultado;
    NodoB* actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }
    while (actual != nullptr) {
        for (const Publicacion& p : actual->publicaciones) {
            resultado.push_back(p);
        }
        actual = actual->siguiente;
    }
    return resultado;
}

void BPlusTree::guardarEnArchivo(const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);
    for (const Publicacion& p : obtenerTodas()) {
        archivo << p.id << "|" << p.idAutor << "|" << p.fecha << "|" << p.contenido << "|";
        for (size_t i = 0; i < p.idsComentarios.size(); i++) {
            archivo << p.idsComentarios[i];
            if (i + 1 < p.idsComentarios.size()) archivo << ",";
        }
        archivo << endl;
    }
    archivo.close();
}

void BPlusTree::cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) return; // no hay archivo todavia, no pasa nada

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string campo;
        Publicacion p;

        getline(ss, campo, '|'); p.id = stoi(campo);
        getline(ss, campo, '|'); p.idAutor = stoi(campo);
        getline(ss, campo, '|'); p.fecha = stoi(campo);
        getline(ss, campo, '|'); p.contenido = campo;
        getline(ss, campo, '|');

        stringstream ssComentarios(campo);
        string idComentario;
        while (getline(ssComentarios, idComentario, ',')) {
            if (!idComentario.empty()) p.idsComentarios.push_back(stoi(idComentario));
        }

        insertar(p);
    }
    archivo.close();
}