#include "TotalizadorErro.hpp"

#include <map>
#include <stdexcept>
#include <string>

AutomatoFinito TotalizadorErro::totalizar(const AutomatoFinito& afd) const {
    if (afd.obterIdEstadoInicial() < 0) {
        throw std::runtime_error("Erro na totalizacao: o AFD nao possui estado inicial.");
    }

    AutomatoFinito afdTotal;

    // Preserva o alfabeto Σ
    for (char simbolo : afd.obterAlfabeto()) {
        afdTotal.adicionarSimboloAlfabeto(simbolo);
    }

    // Copia todos os estados válidos do AFD minimizado
    std::map<int, int> mapaIds;
    bool possuiNomeErro = false;

    for (const auto& estado : afd.obterEstados()) {
        if (estado.nome == "ERRO") {
            possuiNomeErro = true;
        }

        int novoId = afdTotal.criarEstado(
            estado.nome, estado.ehInicial, estado.ehFinal, estado.ehErro);

        if (estado.ehFinal) {
            if (estado.tokensReconhecidos.empty()) {
                afdTotal.marcarComoFinal(novoId, "");
            } else {
                for (const std::string& token : estado.tokensReconhecidos) {
                    afdTotal.marcarComoFinal(novoId, token);
                }
            }
        }

        mapaIds[estado.id] = novoId;
    }

    // Copia as transições definidas existentes
    for (const auto& estado : afd.obterEstados()) {
        int novaOrigem = mapaIds[estado.id];
        for (char simbolo : afd.obterAlfabeto()) {
            for (int destinoAntigo : afd.obterDestinos(estado.id, simbolo)) {
                afdTotal.adicionarTransicao(novaOrigem, simbolo, mapaIds[destinoAntigo]);
            }
        }
    }

    // Cria o Estado de Erro explícito (não-inicial, não-final, marcador ehErro = true)
    std::string nomeEstadoErro = possuiNomeErro ? "Q_ERRO" : "ERRO";
    int idEstadoErro = afdTotal.criarEstado(nomeEstadoErro, false, false, true);

    // Preenche todas as transições indefinidas (incluindo os laços do próprio ERRO)
    for (const auto& estado : afdTotal.obterEstados()) {
        for (char simbolo : afdTotal.obterAlfabeto()) {
            if (afdTotal.obterDestinos(estado.id, simbolo).empty()) {
                afdTotal.adicionarTransicao(estado.id, simbolo, idEstadoErro);
            }
        }
    }

    return afdTotal;
}