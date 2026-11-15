#include "AutomatoFinito.hpp"
#include <algorithm>
#include <stdexcept>

void AutomatoFinito::adicionarSimboloAlfabeto(char simbolo) {
    if (std::find(alfabeto.begin(), alfabeto.end(), simbolo) == alfabeto.end()) {
        alfabeto.push_back(simbolo);
    }
}

const std::vector<char>& AutomatoFinito::obterAlfabeto() const {
    return alfabeto;
}

int AutomatoFinito::criarEstado(const std::string& nome, bool ehInicial,
                                bool ehFinal, bool ehErro) {
    int novoId = static_cast<int>(estados.size());
    Estado estado;
    estado.id = novoId;
    estado.nome = nome;
    estado.ehInicial = ehInicial;
    estado.ehFinal = ehFinal;
    estado.ehErro = ehErro;

    if (ehInicial) {
        idEstadoInicial = novoId;
    }

    estados.push_back(estado);
    return novoId;
}

void AutomatoFinito::marcarComoFinal(int idEstado, const std::string& rotuloToken) {
    Estado& estado = estados.at(idEstado);
    estado.ehFinal = true;

    if (!rotuloToken.empty()) {
        auto& lista = estado.tokensReconhecidos;
        if (std::find(lista.begin(), lista.end(), rotuloToken) == lista.end()) {
            lista.push_back(rotuloToken);
        }
    }
}

const std::vector<Estado>& AutomatoFinito::obterEstados() const {
    return estados;
}

const Estado& AutomatoFinito::obterEstado(int idEstado) const {
    return estados.at(idEstado);
}

int AutomatoFinito::obterIdEstadoInicial() const {
    return idEstadoInicial;
}

void AutomatoFinito::adicionarTransicao(int idOrigem, char simbolo, int idDestino) {
    adicionarSimboloAlfabeto(simbolo);
    transicoes[idOrigem][simbolo].insert(idDestino);
}

std::set<int> AutomatoFinito::obterDestinos(int idOrigem, char simbolo) const {
    auto itOrigem = transicoes.find(idOrigem);
    if (itOrigem == transicoes.end()) {
        return {};
    }

    auto itSimbolo = itOrigem->second.find(simbolo);
    if (itSimbolo == itOrigem->second.end()) {
        return {};
    }

    return itSimbolo->second;
}

bool AutomatoFinito::possuiTransicao(int idOrigem, char simbolo) const {
    return !obterDestinos(idOrigem, simbolo).empty();
}