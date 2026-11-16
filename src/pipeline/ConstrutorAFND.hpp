#ifndef CONSTRUTOR_AFND_HPP
#define CONSTRUTOR_AFND_HPP

#include "../modelo/AutomatoFinito.hpp"
#include "../modelo/EspecificacaoLinguagem.hpp"
#include <string>

// Responsável pela Etapa 2 do pipeline (Construção do AFND):
// unifica tokens literais e Gramáticas Regulares em um único AFND,
// compartilhando exclusivamente o estado inicial 'S' e criando estados
// exclusivos para cada nova transição (Teorema 3.2 da Apostila, pp. 22-23).
class ConstrutorAFND {
private:
    int indiceLetra = 0;
    int indiceExtra = 26;

    // Gera nomes legíveis sequenciais (A, B, ..., R, T, ..., Z, Q26, Q27...)
    // pulando 'S', que é reservado para o estado inicial compartilhado.
    std::string gerarProximoNomeEstado();

    // Constrói uma cadeia linear de estados exclusivos para um token literal
    void adicionarTokenLiteral(AutomatoFinito& afnd, const std::string& token);

    // Converte uma Gramática Regular BNF em estados e transições no AFND
    void adicionarGramaticaRegular(AutomatoFinito& afnd, const GramaticaRegular& gramatica);

public:
    ConstrutorAFND() = default;

    // Executa a construção completa do AFND a partir da especificação lida
    AutomatoFinito construir(const EspecificacaoLinguagem& especificacao);
};

#endif