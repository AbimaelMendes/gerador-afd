#include "Determinizador.hpp"

#include <map>
#include <queue>
#include <stdexcept>

std::string Determinizador::formatarNomeSubconjunto(const AutomatoFinito& afnd,
                                                   const std::set<int>& subconjunto) {
    if (subconjunto.size() == 1) {
        int idUnico = *subconjunto.begin();
        return afnd.obterEstado(idUnico).nome;
    }

    std::string rotulo = "[";
    bool primeiro = true;
    for (int idEstado : subconjunto) {
        if (!primeiro) {
            rotulo += ", ";
        }
        rotulo += afnd.obterEstado(idEstado).nome;
        primeiro = false;
    }
    rotulo += "]";
    return rotulo;
}

AutomatoFinito Determinizador::determinizar(const AutomatoFinito& afnd) const {
    int idInicialAFND = afnd.obterIdEstadoInicial();
    if (idInicialAFND < 0) {
        throw std::runtime_error("Erro na determinizacao: o AFND nao possui estado inicial.");
    }

    AutomatoFinito afd;

    // Preserva exatamente o mesmo alfabeto Σ e a mesma ordem de colunas do AFND
    for (char simbolo : afnd.obterAlfabeto()) {
        afd.adicionarSimboloAlfabeto(simbolo);
    }

    // Mapeia cada subconjunto único de estados do AFND para seu novo ID no AFD
    std::map<std::set<int>, int> mapaSubconjuntos;
    std::queue<std::set<int>> filaProcessamento;

    // O estado inicial do AFD corresponde ao subconjunto unitário {q0}
    std::set<int> subconjuntoInicial = { idInicialAFND };
    int idInicialAFD = afd.criarEstado(formatarNomeSubconjunto(afnd, subconjuntoInicial),
                                       true, false, false);

    mapaSubconjuntos[subconjuntoInicial] = idInicialAFD;
    filaProcessamento.push(subconjuntoInicial);

    // Exploração em largura (BFS) dos subconjuntos alcançáveis a partir de {q0}
    while (!filaProcessamento.empty()) {
        std::set<int> conjuntoAtual = filaProcessamento.front();
        filaProcessamento.pop();

        int idAtualAFD = mapaSubconjuntos[conjuntoAtual];

        // Regra de aceitação (Teorema 3.1): um estado composto é final se ao menos
        // um estado componente for final no AFND. Como std::set<int> mantém os IDs
        // em ordem crescente, tokens literais (criados antes) têm prioridade sobre GRs.
        for (int idEstadoAFND : conjuntoAtual) {
            const Estado& estAFND = afnd.obterEstado(idEstadoAFND);
            if (estAFND.ehFinal) {
                if (estAFND.tokensReconhecidos.empty()) {
                    afd.marcarComoFinal(idAtualAFD, "");
                } else {
                    for (const std::string& token : estAFND.tokensReconhecidos) {
                        afd.marcarComoFinal(idAtualAFD, token);
                    }
                }
            }
        }

        // Calcula δ_AFD(conjuntoAtual, simbolo) = ⋃ δ_AFND(q, simbolo) para todo q ∈ conjuntoAtual
        for (char simbolo : afnd.obterAlfabeto()) {
            std::set<int> proximoSubconjunto;

            for (int idEstadoAFND : conjuntoAtual) {
                std::set<int> destinos = afnd.obterDestinos(idEstadoAFND, simbolo);
                proximoSubconjunto.insert(destinos.begin(), destinos.end());
            }

            // Se o conjunto união for vazio (∅), a transição fica indefinida por enquanto
            // (será direcionada ao Estado de Erro na etapa final do pipeline)
            if (proximoSubconjunto.empty()) {
                continue;
            }

            auto it = mapaSubconjuntos.find(proximoSubconjunto);
            int idDestinoAFD = -1;

            if (it == mapaSubconjuntos.end()) {
                std::string nomeNovo = formatarNomeSubconjunto(afnd, proximoSubconjunto);
                idDestinoAFD = afd.criarEstado(nomeNovo, false, false, false);
                mapaSubconjuntos[proximoSubconjunto] = idDestinoAFD;
                filaProcessamento.push(proximoSubconjunto);
            } else {
                idDestinoAFD = it->second;
            }

            afd.adicionarTransicao(idAtualAFD, simbolo, idDestinoAFD);
        }
    }

    return afd;
}