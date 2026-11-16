#include "ConstrutorAFND.hpp"
#include <map>

std::string ConstrutorAFND::gerarProximoNomeEstado() {
    while (indiceLetra < 26) {
        char letra = static_cast<char>('A' + indiceLetra);
        indiceLetra++;
        // Pula 'S' pois ele já identifica o estado inicial compartilhado
        if (letra != 'S') {
            return std::string(1, letra);
        }
    }
    return "Q" + std::to_string(indiceExtra++);
}

void ConstrutorAFND::adicionarTokenLiteral(AutomatoFinito& afnd, const std::string& token) {
    if (token.empty()) {
        return;
    }

    // Todo token começa a partir do único estado inicial compartilhado 'S'
    int estadoAtual = afnd.obterIdEstadoInicial();

    // Para cada símbolo do token, cria um novo estado destino exclusivo (sem reuso)
    for (char simbolo : token) {
        int proximoEstado = afnd.criarEstado(gerarProximoNomeEstado());
        afnd.adicionarTransicao(estadoAtual, simbolo, proximoEstado);
        estadoAtual = proximoEstado;
    }

    // O último estado alcançado reconhece este token literal
    afnd.marcarComoFinal(estadoAtual, token);
}

void ConstrutorAFND::adicionarGramaticaRegular(AutomatoFinito& afnd,
                                               const GramaticaRegular& gramatica) {
    // Mapeia os não-terminais internos desta gramática para IDs de estados no AFND.
    // Esse mapa é local à gramática, impedindo colisão de nomes entre GRs distintas.
    std::map<std::string, int> mapaEstadosGR;

    // Regra do projeto: o símbolo inicial <S> da GR coincide com o estado inicial global 'S'
    mapaEstadosGR[gramatica.simboloInicial] = afnd.obterIdEstadoInicial();

    // 1ª passada: garante a criação de estados exclusivos para todos os não-terminais da GR
    for (const auto& producao : gramatica.producoes) {
        if (mapaEstadosGR.find(producao.naoTerminalOrigem) == mapaEstadosGR.end()) {
            mapaEstadosGR[producao.naoTerminalOrigem] =
                afnd.criarEstado(gerarProximoNomeEstado());
        }

        if (!producao.ehEpsilon && !producao.naoTerminalDestino.empty()) {
            if (mapaEstadosGR.find(producao.naoTerminalDestino) == mapaEstadosGR.end()) {
                mapaEstadosGR[producao.naoTerminalDestino] =
                    afnd.criarEstado(gerarProximoNomeEstado());
            }
        }
    }

    // 2ª passada: aplica as 3 regras de conversão de GR para AF (Teorema 3.2, Apostila p. 22)
    for (const auto& producao : gramatica.producoes) {
        int idOrigem = mapaEstadosGR[producao.naoTerminalOrigem];

        // Caso A ::= ε -> o estado correspondente a A torna-se estado final
        if (producao.ehEpsilon) {
            afnd.marcarComoFinal(idOrigem, gramatica.nomeIdentificador);
            continue;
        }

        // Caso A ::= a (terminal isolado) -> cria novo estado final exclusivo X e δ(A, a) = X
        if (producao.naoTerminalDestino.empty()) {
            int idNovoFinal = afnd.criarEstado(gerarProximoNomeEstado(), false, true, false);
            afnd.marcarComoFinal(idNovoFinal, gramatica.nomeIdentificador);
            afnd.adicionarTransicao(idOrigem, producao.terminal, idNovoFinal);
            continue;
        }

        // Caso A ::= aB (linear à direita) -> cria transição δ(A, a) = B
        int idDestino = mapaEstadosGR[producao.naoTerminalDestino];
        afnd.adicionarTransicao(idOrigem, producao.terminal, idDestino);
    }
}

AutomatoFinito ConstrutorAFND::construir(const EspecificacaoLinguagem& especificacao) {
    indiceLetra = 0;
    indiceExtra = 26;

    AutomatoFinito afnd;

    // Cria o único estado compartilhado entre todos os tokens e gramáticas: 'S'
    afnd.criarEstado("S", true, false, false);

    // Processa primeiro os tokens literais (garantindo maior prioridade de reconhecimento)
    for (const std::string& token : especificacao.tokensLiterais) {
        adicionarTokenLiteral(afnd, token);
    }

    // Em seguida, processa cada Gramática Regular BNF isoladamente
    for (const GramaticaRegular& gramatica : especificacao.gramaticas) {
        adicionarGramaticaRegular(afnd, gramatica);
    }

    return afnd;
}