#ifndef LEITOR_ARQUIVO_HPP
#define LEITOR_ARQUIVO_HPP

#include "../modelo/EspecificacaoLinguagem.hpp"
#include <string>
#include <vector>

// Responsável pela Etapa 1 do pipeline (Ingestão e Interpretação):
// lê o arquivo texto de entrada e separa cadeias literais (tokens)
// de produções de Gramáticas Regulares escritas em notação BNF.
class LeitorArquivo {
private:
    // Remove espaços em branco, tabulações e quebras de linha (\r, \n) das extremidades
    static std::string removerEspacos(const std::string& texto);

    // Extrai o identificador interno de um não-terminal escrito entre '<' e '>'
    // Ex.: "<S>" -> "S"
    static std::string extrairNomeNaoTerminal(const std::string& texto, int numeroLinha);

    // Verifica se uma alternativa representa a cadeia vazia (ε, & ou epsilon)
    static bool ehSimboloEpsilon(const std::string& texto);

    // Quebra o lado direito de uma regra BNF pelo separador '|'
    static std::vector<std::string> dividirAlternativas(const std::string& ladoDireito);

    // Converte uma alternativa textual (ex.: "a<A>", "a" ou "ε") em uma ProducaoBNF
    static ProducaoBNF interpretarAlternativa(const std::string& origem,
                                              const std::string& alternativa,
                                              int numeroLinha);

public:
    LeitorArquivo() = default;

    // Lê o arquivo no caminho informado e retorna a especificação estruturada
    EspecificacaoLinguagem lerArquivo(const std::string& caminhoArquivo) const;
};

#endif