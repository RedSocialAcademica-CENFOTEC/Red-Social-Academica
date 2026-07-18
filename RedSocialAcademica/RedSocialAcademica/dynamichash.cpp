#include "dynamichash.h"
#include <fstream>
#include <sstream>

Bucket::Bucket(int profLocal, int cap) : profundidadLocal(profLocal), capacidad(cap) {}

DynamicHash::DynamicHash() : profundidadGlobal(1) {
    Bucket* bucketInicial = new Bucket(1);
    directorio.push_back(bucketInicial);
    directorio.push_back(bucketInicial); // los dos apuntan al mismo bucket al arrancar
}

int DynamicHash::calcularHash(const string& clave) {
    int h = 0;
    for (char c : clave) {
        h = h * 31 + c; // hash simple tipo polinomial
    }
    if (h < 0) h = -h;
    return h % (1 << profundidadGlobal); // mod 2^profundidadGlobal, como en el diagrama
}

void DynamicHash::dividirBucket(int indice) {
    Bucket* bucketViejo = directorio[indice];

    if (bucketViejo->profundidadLocal == profundidadGlobal) {
        // el directorio esta muy pq, hay que duplicarlo
        int tamanioViejo = (int)directorio.size();
        for (int i = 0; i < tamanioViejo; i++) {
            directorio.push_back(directorio[i]);
        }
        profundidadGlobal++;
    }

    int nuevaProfundidad = bucketViejo->profundidadLocal + 1;
    Bucket* bucketNuevo = new Bucket(nuevaProfundidad);
    bucketViejo->profundidadLocal = nuevaProfundidad;

    // La mitad de los punteros que apuntaban al bucket viejo pasan al nuevo
    int mascara = 1 << (nuevaProfundidad - 1);
    for (size_t i = 0; i < directorio.size(); i++) {
        if (directorio[i] == bucketViejo && (i & mascara)) {
            directorio[i] = bucketNuevo;
        }
    }

    // Reubicamos los usuarios que ya estaban en el bucket viejo
    vector<Usuario> usuariosOriginales = bucketViejo->usuarios;
    bucketViejo->usuarios.clear();

    for (const Usuario& u : usuariosOriginales) {
        int idx = calcularHash(u.nombre);
        directorio[idx]->usuarios.push_back(u);
    }
}

void DynamicHash::insertar(const Usuario& user) {
    int idx = calcularHash(user.nombre);
    Bucket* bucket = directorio[idx];

    if ((int)bucket->usuarios.size() < bucket->capacidad) {
        bucket->usuarios.push_back(user);
        return;
    }

    dividirBucket(idx);
    insertar(user); // reintentamos, ahora con el directorio ya dividido
}

/*
Cuando la profundidad local sube en 1, hay un bit nuevo que decide a cuál de los dos buckets (viejo o nuevo)
le toca cada índice del directorio. mascara marca justo ese bit nuevo — si está prendido en el índice i, ese puntero pasa al bucket nuevo; si no, se queda con el viejo.
*/


//Buscar por nombre es rápida como el hash se calcula justo sobre el nombre, sabés exactamente a qué bucket ir sin revisar nada más.
Usuario* DynamicHash::buscarPorNombre(const string& nombre) {
    int idx = calcularHash(nombre);
    Bucket* bucket = directorio[idx];

    for (Usuario& u : bucket->usuarios) {
        if (u.nombre == nombre) {
            return &u;
        }
    }
    return nullptr; // no encontrado
}

//buscarPorCarrera y buscarPorInstitucion son más lentas — el hash no tiene idea de carreras ni 
// instituciones, solo de nombres. Entonces para buscar por esos campos no hay atajo: hay que recorrer todos los buckets, uno por uno, 
// revisando usuario por usuario


vector<Usuario> DynamicHash::buscarPorCarrera(const string& carrera) {
    vector<Usuario> resultado;
    vector<Bucket*> visitados;

    for (Bucket* b : directorio) {
        bool yaVisitado = false;
        for (Bucket* v : visitados) {
            if (v == b) { yaVisitado = true; break; }
        }
        if (yaVisitado) continue;
        visitados.push_back(b);

        for (const Usuario& u : b->usuarios) {
            if (u.carrera == carrera) {
                resultado.push_back(u);
            }
        }
    }
    return resultado;
}

vector<Usuario> DynamicHash::buscarPorInstitucion(const string& institucion) {
    vector<Usuario> resultado;
    vector<Bucket*> visitados;

    for (Bucket* b : directorio) {
        bool yaVisitado = false;
        for (Bucket* v : visitados) {
            if (v == b) { yaVisitado = true; break; }
        }
        if (yaVisitado) continue;
        visitados.push_back(b);

        for (const Usuario& u : b->usuarios) {
            if (u.institucion == institucion) {
                resultado.push_back(u);
            }
        }
    }
    return resultado;
}

vector<Usuario> DynamicHash::obtenerTodos() {
    vector<Usuario> resultado;
    vector<Bucket*> visitados;

    for (Bucket* b : directorio) {
        bool yaVisitado = false;
        for (Bucket* v : visitados) if (v == b) { yaVisitado = true; break; }
        if (yaVisitado) continue;
        visitados.push_back(b);

        for (const Usuario& u : b->usuarios) resultado.push_back(u);
    }
    return resultado;
}

void DynamicHash::guardarEnArchivo(const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);
    for (const Usuario& u : obtenerTodos()) {
        archivo << u.id << "|" << u.nombre << "|" << u.correo << "|"
            << u.carrera << "|" << u.institucion << "|" << (int)u.tipo << endl;
    }
    archivo.close();
}

void DynamicHash::cargarDesdeArchivo(const string& nombreArchivo) {
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

        insertar(u);
    }
    archivo.close();
}