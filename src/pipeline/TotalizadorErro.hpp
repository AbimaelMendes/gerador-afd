#ifndef TOTALIZADOR_ERRO_HPP
#define TOTALIZADOR_ERRO_HPP

#include "../modelo/AutomatoFinito.hpp"

// Responsável pela Etapa 5 do pipeline (Totalização / Inclusão do Estado de Erro):
// transforma a função de transição parcial δ em uma função total δ: K × Σ -> K,
// adicionando um estado absorvedor de erro (ERRO ∉ F) para todas as transições
// indefinidas (Apostila, Cap. III).
class TotalizadorErro {
public:
    TotalizadorErro() = default;

    // Retorna o AFD completo e totalizado, pronto para a simulação léxica
    AutomatoFinito totalizar(const AutomatoFinito& afd) const;
};

#endif