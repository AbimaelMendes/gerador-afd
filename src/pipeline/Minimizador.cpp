#include "Minimizador.hpp"

#include <map>
#include <queue>
#include <stdexcept>

std::set<int> Minimizador::encontrarEstadosAlcancaveis(const AutomatoFinito& afd) {
    std::set<int> alcancaveis;
    int idInicial = afd.obterIdEstadoInicial();
    if (idInicial < 0) {
        return alcancaveis;
    }

    std::queue<int> fila;
    alcancaveis.insert(idInicial);
    fila.push(idInicial);

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();

        for (char simbolo : afd.obterAlfabeto()) {
            for (int destino : afd.obterDestinos(atual, simbolo)) {
                if (alcancaveis.find(destino) == alcancaveis.end()) {
                    alcancaveis.insert(destino);
                    fila.push(destino);
                }
            }
        }
    }

    return alcancaveis;
}

std::set<int> Minimizador::encontrarEstadosVivos(const AutomatoFinito& afd) {
    // Constrói o grafo reverso de transições (destino -> conjunto de origens)
    std::map<int, std::set<int>> grafoReverso;
    for (const auto& estado : afd.obterEstados()) {
        for (char simbolo : afd.obterAlfabeto()) {
            for (int destino : afd.obterDestinos(estado.id, simbolo)) {
                grafoReverso[destino].insert(estado.id);
            }
        }
    }

    // Inicia a busca reversa a partir de todos os estados finais (F)
    std::set<int> vivos;
    std::queue<int> fila;

    for (const auto& estado : afd.obterEstados()) {
        if (estado.ehFinal) {
            vivos.insert(estado.id);
            fila.push(estado.id);
        }
    }

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();

        auto it = grafoReverso.find(atual);
        if (it != grafoReverso.end()) {
            for (int antecessor : it->second) {
                if (vivos.find(antecessor) == vivos.end()) {
                    vivos.insert(antecessor);
                    fila.push(antecessor);
                }
            }
        }
    }

    return vivos;
}

AutomatoFinito Minimizador::minimizar(const AutomatoFinito& afd) const {
    int idInicial = afd.obterIdEstadoInicial();
    if (idInicial < 0) {
        throw std::runtime_error("Erro na minimizacao: o AFD nao possui estado inicial.");
    }

    std::set<int> alcancaveis = encontrarEstadosAlcancaveis(afd);
    std::set<int> vivos = encontrarEstadosVivos(afd);

    AutomatoFinito afdMinimizado;

    // Preserva o alfabeto Σ original
    for (char simbolo : afd.obterAlfabeto()) {
        afdMinimizado.adicionarSimboloAlfabeto(simbolo);
    }

    // 1ª passada: copia apenas os estados que são simultaneamente alcançáveis e vivos.
    // O estado inicial q0 é sempre preservado para manter a quíntupla válida, mesmo no
    // caso extremo de uma linguagem vazia (F = ∅).
    std::map<int, int> mapaIds;
    for (const auto& estado : afd.obterEstados()) {
        bool ehAlcancavel = alcancaveis.find(estado.id) != alcancaveis.end();
        bool ehVivo = vivos.find(estado.id) != vivos.end();

        if ((ehAlcancavel && ehVivo) || estado.id == idInicial) {
            int novoId = afdMinimizado.criarEstado(
                estado.nome, estado.ehInicial, estado.ehFinal, estado.ehErro);

            if (estado.ehFinal) {
                if (estado.tokensReconhecidos.empty()) {
                    afdMinimizado.marcarComoFinal(novoId, "");
                } else {
                    for (const std::string& token : estado.tokensReconhecidos) {
                        afdMinimizado.marcarComoFinal(novoId, token);
                    }
                }
            }

            mapaIds[estado.id] = novoId;
        }
    }

    // 2ª passada: reconstrói as transições entre os estados sobreviventes.
    // Transições que apontavam para estados mortos são descartadas e serão
    // redirecionadas para o Estado de Erro na etapa seguinte.
    for (const auto& estado : afd.obterEstados()) {
        auto itOrigem = mapaIds.find(estado.id);
        if (itOrigem == mapaIds.end()) {
            continue;
        }

        int novaOrigem = itOrigem->second;
        for (char simbolo : afd.obterAlfabeto()) {
            for (int destinoAntigo : afd.obterDestinos(estado.id, simbolo)) {
                auto itDestino = mapaIds.find(destinoAntigo);
                if (itDestino != mapaIds.end()) {
                    afdMinimizado.adicionarTransicao(novaOrigem, simbolo, itDestino->second);
                }
            }
        }
    }

    return afdMinimizado;
}