#ifndef MINIMIZADOR_HPP
#define MINIMIZADOR_HPP

#include "../modelo/AutomatoFinito.hpp"
#include <set>

// Responsável pela Etapa 4 do pipeline (Minimização / Poda de Estados Inúteis):
// remove estados inalcançáveis a partir do estado inicial q0 e estados mortos
// que não possuem caminho até algum estado final em F (Apostila, Cap. III).
class Minimizador {
private:
    // Busca em largura (BFS) direta a partir de q0 para identificar estados alcançáveis
    static std::set<int> encontrarEstadosAlcancaveis(const AutomatoFinito& afd);

    // Busca em largura (BFS) reversa a partir de F para identificar estados vivos (úteis)
    static std::set<int> encontrarEstadosVivos(const AutomatoFinito& afd);

public:
    Minimizador() = default;

    // Retorna um novo AFD equivalente contendo apenas estados úteis (alcançáveis e vivos)
    AutomatoFinito minimizar(const AutomatoFinito& afd) const;
};

#endif