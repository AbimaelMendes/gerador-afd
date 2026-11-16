#ifndef DETERMINIZADOR_HPP
#define DETERMINIZADOR_HPP

#include "../modelo/AutomatoFinito.hpp"
#include <set>
#include <string>

// Responsável pela Etapa 3 do pipeline (Determinização):
// converte um AFND em um AFD equivalente utilizando o algoritmo de
// Construção de Subconjuntos (Teorema 3.1 da Apostila, pp. 20-21),
// preservando a precedência dos tokens reconhecidos nos estados finais.
class Determinizador {
private:
    // Gera o rótulo legível de um estado composto do AFD a partir de um
    // subconjunto de estados do AFND (ex.: "S" para unitário ou "[A, H]" para composto)
    static std::string formatarNomeSubconjunto(const AutomatoFinito& afnd,
                                               const std::set<int>& subconjunto);

public:
    Determinizador() = default;

    // Executa a determinização e retorna o novo AutomatoFinito determinístico
    AutomatoFinito determinizar(const AutomatoFinito& afnd) const;
};

#endif