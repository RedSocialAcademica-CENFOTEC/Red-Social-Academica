#include "dynamichash.h"
#include <algorithm>
#include <cmath>

DynamicHash::DynamicHash(int _capacidadBucket)
    : profundidadGlobal(1), capacidadBucket(_capacidadBucket) {
    if (capacidadBucket < 1) capacidadBucket = 4;
    directorio.resize(2);
    for (auto& bucket : directorio) {
        bucket = new Bucket();
    }
}

DynamicHash::~DynamicHash() {
    for (auto bucket : directorio) {
        delete bucket;
    }
}

size_t DynamicHash::hashNombre(const string& nombre) const {
    return hash<string>{}(nombre);
}

size_t DynamicHash::hashCarrera(const string& carrera) const {
    return hash<string>{}(carrera);
}

size_t DynamicHash::hashInstitucion(const string& institucion) const {
    return hash<string>{}(institucion);
}

size_t DynamicHash::indiceHash(const string& clave, int prof) const {
    size_t hashVal = hash<string>{}(clave);
    // Usar los últimos 'prof' bits
    return hashVal & ((1 << prof) - 1);
}

void DynamicHash::insertar(const Usuario& usuario) {
    size_t indice = indiceHash(usuario.nombre, profundidadGlobal);
    auto bucket = directorio[indice];

    // Verificar si el bucket está lleno
    if (bucket->datos.size() < capacidadBucket) {
        bucket->datos.push_back(usuario);
        return;
    }

    // Bucket lleno, dividir
    if (bucket->profundidadLocal == profundidadGlobal) {
        duplicarDirectorio();
    }

    // Dividir el bucket
    dividirBucket(indice);

    // Reinsertar el usuario
    indice = indiceHash(usuario.nombre, profundidadGlobal);
    insertar(usuario);
}

void DynamicHash::duplicarDirectorio() {
    int oldSize = directorio.size();
    int newSize = oldSize * 2;
    directorio.resize(newSize);

    // Copiar las referencias
    for (int i = 0; i < oldSize; i++) {
        directorio[i + oldSize] = directorio[i];
    }

    profundidadGlobal++;
}

void DynamicHash::dividirBucket(int indice) {
    auto bucketViejo = directorio[indice];
    bucketViejo->profundidadLocal++;

    // Crear nuevo bucket
    auto nuevoBucket = new Bucket();
    nuevoBucket->profundidadLocal = bucketViejo->profundidadLocal;

    // Reorganizar el directorio
    int bit = 1 << (bucketViejo->profundidadLocal - 1);
    for (int i = 0; i < directorio.size(); i++) {
        if ((i & (bit - 1)) == (indice & (bit - 1))) {
            if ((i & bit) != 0) {
                directorio[i] = nuevoBucket;
            }
        }
    }

    // Reubicar elementos del bucket viejo
    reubicarBucket(indice);
}

void DynamicHash::reubicarBucket(int indice) {
    auto bucketViejo = directorio[indice];
    vector<Usuario> elementosViejos = bucketViejo->datos;
    bucketViejo->datos.clear();

    // Reinsertar elementos en su nuevo bucket
    for (const auto& usuario : elementosViejos) {
        size_t nuevoIndice = indiceHash(usuario.nombre, profundidadGlobal);
        directorio[nuevoIndice]->datos.push_back(usuario);
    }
}

vector<Usuario> DynamicHash::buscarPorNombre(const string& nombre) const {
    size_t indice = indiceHash(nombre, profundidadGlobal);
    auto bucket = directorio[indice];

    vector<Usuario> resultado;
    for (const auto& usuario : bucket->datos) {
        if (usuario.nombre == nombre) {
            resultado.push_back(usuario);
        }
    }
    return resultado;
}

vector<Usuario> DynamicHash::buscarPorCarrera(const string& carrera) const {
    vector<Usuario> resultado;
    for (auto bucket : directorio) {
        for (const auto& usuario : bucket->datos) {
            if (usuario.carrera == carrera) {
                resultado.push_back(usuario);
            }
        }
    }
    return resultado;
}

vector<Usuario> DynamicHash::buscarPorInstitucion(const string& institucion) const {
    vector<Usuario> resultado;
    for (auto bucket : directorio) {
        for (const auto& usuario : bucket->datos) {
            if (usuario.institucion == institucion) {
                resultado.push_back(usuario);
            }
        }
    }
    return resultado;
}

vector<Usuario> DynamicHash::buscar(const string& campo, const string& valor) const {
    if (campo == "nombre") return buscarPorNombre(valor);
    if (campo == "carrera") return buscarPorCarrera(valor);
    if (campo == "institucion") return buscarPorInstitucion(valor);
    return vector<Usuario>();
}

bool DynamicHash::guardar(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Guardar metadatos
    archivo.write(reinterpret_cast<const char*>(&profundidadGlobal), sizeof(profundidadGlobal));
    archivo.write(reinterpret_cast<const char*>(&capacidadBucket), sizeof(capacidadBucket));

    size_t tamanio = directorio.size();
    archivo.write(reinterpret_cast<const char*>(&tamanio), sizeof(tamanio));

    // Guardar cada bucket
    for (size_t i = 0; i < tamanio; i++) {
        auto bucket = directorio[i];
        archivo.write(reinterpret_cast<const char*>(&bucket->profundidadLocal), sizeof(bucket->profundidadLocal));

        size_t numDatos = bucket->datos.size();
        archivo.write(reinterpret_cast<const char*>(&numDatos), sizeof(numDatos));

        for (const auto& usuario : bucket->datos) {
            // Serializar usuario - simplificado
            size_t len = usuario.id.length();
            archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
            archivo.write(usuario.id.c_str(), len);

            len = usuario.nombre.length();
            archivo.write(reinterpret_cast<const char*>(&len), sizeof(len));
            archivo.write(usuario.nombre.c_str(), len);

            // ... continuar con todos los campos
        }
    }

    archivo.close();
    return true;
}

bool DynamicHash::cargar(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;

    // Limpiar directorio actual
    for (auto bucket : directorio) {
        delete bucket;
    }

    // Cargar metadatos
    archivo.read(reinterpret_cast<char*>(&profundidadGlobal), sizeof(profundidadGlobal));
    archivo.read(reinterpret_cast<char*>(&capacidadBucket), sizeof(capacidadBucket));

    size_t tamanio;
    archivo.read(reinterpret_cast<char*>(&tamanio), sizeof(tamanio));

    directorio.resize(tamanio);
    for (size_t i = 0; i < tamanio; i++) {
        directorio[i] = new Bucket();
        archivo.read(reinterpret_cast<char*>(&directorio[i]->profundidadLocal), sizeof(directorio[i]->profundidadLocal));

        size_t numDatos;
        archivo.read(reinterpret_cast<char*>(&numDatos), sizeof(numDatos));

        for (size_t j = 0; j < numDatos; j++) {
            Usuario usuario;
            size_t len;

            archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
            usuario.id.resize(len);
            archivo.read(&usuario.id[0], len);

            archivo.read(reinterpret_cast<char*>(&len), sizeof(len));
            usuario.nombre.resize(len);
            archivo.read(&usuario.nombre[0], len);

            // ... continuar cargando todos los campos

            directorio[i]->datos.push_back(usuario);
        }
    }

    archivo.close();
    return true;
}

void DynamicHash::imprimir() const {
    cout << "=== Hashing Dinámico ===" << endl;
    cout << "Profundidad Global: " << profundidadGlobal << endl;
    cout << "Capacidad por Bucket: " << capacidadBucket << endl;
    cout << "Directorio: " << directorio.size() << " buckets" << endl;

    for (size_t i = 0; i < directorio.size(); i++) {
        cout << "Bucket " << i << " (prof. " << directorio[i]->profundidadLocal << "): ";
        for (const auto& usuario : directorio[i]->datos) {
            cout << usuario.nombre << " ";
        }
        cout << endl;
    }
    cout << "========================" << endl;
}

int DynamicHash::size() const {
    int total = 0;
    for (auto bucket : directorio) {
        total += bucket->datos.size();
    }
    return total;
}

bool DynamicHash::vacio() const {
    return size() == 0;
}