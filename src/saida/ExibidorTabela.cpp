#include "ExibidorTabela.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

std::string ExibidorTabela::formatarRotuloEstado(const Estado& estado) {
    std::string prefixo;
    if (estado.ehInicial && estado.ehFinal) {
        prefixo = "->*";
    } else if (estado.ehInicial) {
        prefixo = "-> ";
    } else if (estado.ehFinal) {
        prefixo = " * ";
    } else {
        prefixo = "   ";
    }
    return prefixo + estado.nome;
}

std::string ExibidorTabela::formatarDestinos(const AutomatoFinito& automato,
                                             int idOrigem, char simbolo) {
    std::set<int> destinos = automato.obterDestinos(idOrigem, simbolo);
    if (destinos.empty()) {
        return "-";
    }

    std::string resultado;
    bool primeiro = true;
    for (int idDestino : destinos) {
        if (!primeiro) {
            resultado += ", ";
        }
        resultado += automato.obterEstado(idDestino).nome;
        primeiro = false;
    }
    return resultado;
}

std::string ExibidorTabela::formatarTokensReconhecidos(const Estado& estado) {
    if (estado.ehErro) {
        return "[ERRO LEXICO]";
    }
    if (!estado.ehFinal || estado.tokensReconhecidos.empty()) {
        return "";
    }

    std::string texto;
    for (size_t i = 0; i < estado.tokensReconhecidos.size(); ++i) {
        if (i > 0) {
            texto += ", ";
        }
        texto += estado.tokensReconhecidos[i];
    }
    return texto;
}

void ExibidorTabela::exibir(const std::string& tituloEtapa,
                            const AutomatoFinito& automato) const {
    const auto& estados = automato.obterEstados();
    const auto& alfabeto = automato.obterAlfabeto();

    std::cout << "\n====================================================================\n";
    std::cout << " " << tituloEtapa << "\n";
    std::cout << "====================================================================\n";

    if (estados.empty()) {
        std::cout << " (Automato vazio)\n";
        return;
    }

    // Calcula larguras dinâmicas das colunas para manter o alinhamento visual
    size_t larguraColunaEstado = std::string("Estado").size();
    for (const auto& est : estados) {
        larguraColunaEstado = std::max(larguraColunaEstado,
                                       formatarRotuloEstado(est).size());
    }
    larguraColunaEstado += 2;

    std::vector<size_t> largurasSimbolos(alfabeto.size(), 3);
    for (size_t c = 0; c < alfabeto.size(); ++c) {
        for (const auto& est : estados) {
            std::string celula = formatarDestinos(automato, est.id, alfabeto[c]);
            largurasSimbolos[c] = std::max(largurasSimbolos[c], celula.size());
        }
        largurasSimbolos[c] += 2;
    }

    // Cabeçalho da tabela
    std::cout << std::left << std::setw(static_cast<int>(larguraColunaEstado)) << "Estado";
    for (size_t c = 0; c < alfabeto.size(); ++c) {
        std::cout << "| " << std::left
                  << std::setw(static_cast<int>(largurasSimbolos[c]))
                  << std::string(1, alfabeto[c]);
    }
    std::cout << "| Reconhecimento\n";

    // Linha separadora
    std::cout << std::string(larguraColunaEstado, '-');
    for (size_t c = 0; c < alfabeto.size(); ++c) {
        std::cout << "+" << std::string(largurasSimbolos[c] + 1, '-');
    }
    std::cout << "+-------------------\n";

    // Linhas de cada estado
    for (const auto& est : estados) {
        std::cout << std::left << std::setw(static_cast<int>(larguraColunaEstado))
                  << formatarRotuloEstado(est);

        for (size_t c = 0; c < alfabeto.size(); ++c) {
            std::string celula = formatarDestinos(automato, est.id, alfabeto[c]);
            std::cout << "| " << std::left
                      << std::setw(static_cast<int>(largurasSimbolos[c]))
                      << celula;
        }

        std::cout << "| " << formatarTokensReconhecidos(est) << "\n";
    }
    std::cout << "====================================================================\n";
}