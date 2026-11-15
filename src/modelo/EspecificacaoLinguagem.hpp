#ifndef ESPECIFICACAO_LINGUAGEM_HPP
#define ESPECIFICACAO_LINGUAGEM_HPP

#include <string>
#include <vector>

// Representa uma única regra de produção de uma Gramática Regular (GR).
// Na hierarquia de Chomsky (Tipo 3), toda produção assume uma das três formas:
// 1) <Origem> ::= terminal<Destino>   (ex: <S> ::= a<A>)
// 2) <Origem> ::= terminal            (ex: <S> ::= a)
// 3) <Origem> ::= ε                   (ex: <A> ::= ε)
struct ProducaoBNF {
    std::string naoTerminalOrigem;
    char terminal = '\0';
    std::string naoTerminalDestino; // Vazio se for produção terminal pura ou ε
    bool ehEpsilon = false;
};

// Agrupa todas as produções pertencentes a uma mesma Gramática Regular do arquivo.
struct GramaticaRegular {
    std::string nomeIdentificador;  // Ex: "GR_1", "GR_2"
    std::string simboloInicial;     // Primeiro não-terminal declarado na GR (ex: "S")
    std::vector<ProducaoBNF> producoes;
};

// Estrutura intermediária que concentra tudo o que foi lido do arquivo de entrada.
struct EspecificacaoLinguagem {
    std::vector<std::string> tokensLiterais;        // Ex: "se", "entao", "senao"
    std::vector<GramaticaRegular> gramaticas;       // Lista de GRs em notação BNF
};

#endif