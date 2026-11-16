#ifndef EXIBIDOR_TABELA_HPP
#define EXIBIDOR_TABELA_HPP

#include "../modelo/AutomatoFinito.hpp"
#include <string>

// Responsável por apresentar no terminal a tabela de transições δ
// de qualquer autômato ao longo das etapas do pipeline, seguindo a
// convenção visual da apostila ('->' para inicial, '*' para finais).
class ExibidorTabela {
private:
    static std::string formatarRotuloEstado(const Estado& estado);
    static std::string formatarDestinos(const AutomatoFinito& automato,
                                        int idOrigem, char simbolo);
    static std::string formatarTokensReconhecidos(const Estado& estado);

public:
    ExibidorTabela() = default;

    void exibir(const std::string& tituloEtapa, const AutomatoFinito& automato) const;
};

#endif