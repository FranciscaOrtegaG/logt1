typedef long long ll;
using namespace std;
#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <unordered_map>

const int PAGE_SIZE = 1024;

const int maxElementosPorPagina = PAGE_SIZE/64;

struct page{
    vector<ll> elementos;
    page* next;

    page(): next(nullptr) {};
};

ll h(ll y) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<ll> dis(0, (1LL << 64) - 1);
    return dis(gen);
}

// Estructura llave -> valor
struct Entry {
    unsigned long long int key;
    vector<page> listPages; // Lista de paginas
};


// Tabla de hashing
class tablaHashing{
    public:

        vector<Entry> table; // Vector que representa la tabla hash
        ll p; // cantidad de paginas
        ll t; // 

        // Constructor
        tablaHashing() {
            ll p = 1;
            ll t = 0;
        };
        
        // Insertar
        void insert(ll valor) {
            int k = h(valor) % (1 << (t + 1));
            if (k < p){
                insertarEnPagina(k, valor);
            }
            else{
                insertarEnPagina(k-(1<<t), valor);
            }
        }

        // Buscar en página
        bool buscar_en_pagina(page* pag, ll y){
            for(int i = 0; i < maxElementosPorPagina; i++){
                if(pag->elementos[i] == y){
                    return true;
                }
            }
            return false;
        }
        
        bool buscar(ll y) {
            for (int i = 1; i < p; i++) {
                for (int j =0; j < table[i].listPages.size(); j++){
                    if (buscar_en_pagina(&table[i].listPages[j], y)) {
                        return true;
                    }
                }
            }
            return false;   
        }

        void insertarEnPagina(ll idPage,ll valor){
            page* pag = &table[idPage];
            while(pag->next != nullptr){
                pag = pag->next;
            }
            if(pag->elementos.size() < maxElementosPorPagina){
                pag->elementos.push_back(valor);
            }
            else{
                page* newPage = new page();
                newPage->elementos.push_back(valor);
                pag->next = newPage;
            }
            
        }


        void print() {
            cout << "Tabla Hash:\n";
            for (int i = 0; i < table.size(); i++) {
                Entry e = table[i];
                cout << e.key << "-" << e.value << "\n";
            }
        }

};