#include "LeitorArquivo.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::string LeitorArquivo::removerEspacos(const std::string& texto) {
    const std::string espacos = " \t\r\n";
    size_t inicio = texto.find_first_not_of(espacos);
    if (inicio == std::string::npos) {
        return "";
    }
    size_t fim = texto.find_last_not_of(espacos);
    return texto.substr(inicio, fim - inicio + 1);
}

std::string LeitorArquivo::extrairNomeNaoTerminal(const std::string& texto, int numeroLinha) {
    std::string limpo = removerEspacos(texto);
    if (limpo.size() < 3 || limpo.front() != '<' || limpo.back() != '>') {
        throw std::runtime_error("Erro sintatico BNF na linha " + std::to_string(numeroLinha) +
                                 ": nao-terminal invalido '" + limpo + "' (esperado formato <X>).");
    }
    return removerEspacos(limpo.substr(1, limpo.size() - 2));
}

bool LeitorArquivo::ehSimboloEpsilon(const std::string& texto) {
    // Aceita 'ε' em UTF-8 (\xCE\xB5), além de '&' e "epsilon" para compatibilidade de codificação
    return texto == "ε" || texto == "\xCE\xB5" || texto == "&" || texto == "epsilon";
}

std::vector<std::string> LeitorArquivo::dividirAlternativas(const std::string& ladoDireito) {
    std::vector<std::string> alternativas;
    std::stringstream fluxo(ladoDireito);
    std::string trecho;

    while (std::getline(fluxo, trecho, '|')) {
        std::string limpo = removerEspacos(trecho);
        if (!limpo.empty()) {
            alternativas.push_back(limpo);
        }
    }
    return alternativas;
}

ProducaoBNF LeitorArquivo::interpretarAlternativa(const std::string& origem,
                                                  const std::string& alternativa,
                                                  int numeroLinha) {
    ProducaoBNF producao;
    producao.naoTerminalOrigem = origem;

    // Forma 3 da Gramática Regular: <Origem> ::= ε
    if (ehSimboloEpsilon(alternativa)) {
        producao.ehEpsilon = true;
        producao.terminal = '\0';
        producao.naoTerminalDestino = "";
        return producao;
    }

    // Nas formas <Origem> ::= a<Destino> ou <Origem> ::= a, o primeiro caractere é o terminal
    producao.terminal = alternativa[0];
    producao.ehEpsilon = false;

    std::string restante = removerEspacos(alternativa.substr(1));

    // Forma 2 da Gramática Regular: <Origem> ::= a (produção terminal pura)
    if (restante.empty()) {
        producao.naoTerminalDestino = "";
        return producao;
    }

    // Forma 1 da Gramática Regular: <Origem> ::= a<Destino> (linear à direita)
    producao.naoTerminalDestino = extrairNomeNaoTerminal(restante, numeroLinha);
    return producao;
}

EspecificacaoLinguagem LeitorArquivo::lerArquivo(const std::string& caminhoArquivo) const {
    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de entrada: " + caminhoArquivo);
    }

    EspecificacaoLinguagem especificacao;
    GramaticaRegular gramaticaAtual;
    bool construindoGramatica = false;
    bool houveSeparacaoBloco = false;
    int contadorGramaticas = 0;

    auto finalizarGramaticaAtual = [&]() {
        if (construindoGramatica && !gramaticaAtual.producoes.empty()) {
            especificacao.gramaticas.push_back(gramaticaAtual);
            gramaticaAtual = GramaticaRegular{};
            construindoGramatica = false;
        }
    };

    std::string linhaBruta;
    int numeroLinha = 0;

    while (std::getline(arquivo, linhaBruta)) {
        numeroLinha++;
        std::string linha = removerEspacos(linhaBruta);

        // Linhas vazias apenas sinalizam possível separação entre blocos de gramáticas
        if (linha.empty()) {
            if (construindoGramatica) {
                houveSeparacaoBloco = true;
            }
            continue;
        }

        size_t posDefinicao = linha.find("::=");

        // Caso 1: Linha não contém "::=", portanto é um Token Literal (palavra reservada, operador, etc.)
        if (posDefinicao == std::string::npos) {
            finalizarGramaticaAtual();
            especificacao.tokensLiterais.push_back(linha);
            houveSeparacaoBloco = false;
            continue;
        }

        // Caso 2: Linha contém "::=", portanto é uma produção BNF de Gramática Regular
        std::string ladoEsquerdo = linha.substr(0, posDefinicao);
        std::string ladoDireito = linha.substr(posDefinicao + 3);

        std::string origem = extrairNomeNaoTerminal(ladoEsquerdo, numeroLinha);

        // Verifica se a gramática atual já possui outros não-terminais além do inicial
        bool possuiOutrosNaoTerminais = false;
        for (const auto& prod : gramaticaAtual.producoes) {
            if (prod.naoTerminalOrigem != gramaticaAtual.simboloInicial) {
                possuiOutrosNaoTerminais = true;
                break;
            }
        }

        // Inicia uma nova Gramática Regular se não houver uma ativa ou se um novo <S>
        // aparecer após separação de bloco / após a definição dos estados internos da GR anterior
        if (!construindoGramatica ||
            (origem == gramaticaAtual.simboloInicial && (houveSeparacaoBloco || possuiOutrosNaoTerminais))) {
            finalizarGramaticaAtual();
            contadorGramaticas++;
            gramaticaAtual.nomeIdentificador = "GR_" + std::to_string(contadorGramaticas);
            gramaticaAtual.simboloInicial = origem;
            construindoGramatica = true;
        }

        houveSeparacaoBloco = false;

        std::vector<std::string> alternativas = dividirAlternativas(ladoDireito);
        if (alternativas.empty()) {
            throw std::runtime_error("Erro sintatico BNF na linha " + std::to_string(numeroLinha) +
                                     ": nenhuma producao encontrada apos '::='.");
        }

        for (const std::string& alt : alternativas) {
            ProducaoBNF prod = interpretarAlternativa(origem, alt, numeroLinha);
            gramaticaAtual.producoes.push_back(prod);
        }
    }

    finalizarGramaticaAtual();
    return especificacao;
}