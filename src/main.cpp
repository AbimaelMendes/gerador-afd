#include "entrada/LeitorArquivo.hpp"
#include "pipeline/ConstrutorAFND.hpp"
#include "pipeline/Determinizador.hpp"
#include "saida/ExibidorTabela.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string caminhoEntrada = "entradas/exemplo_pdf.txt";
    if (argc > 1) {
        caminhoEntrada = argv[1];
    }

    try {
        // Etapa 1: Leitura do arquivo fonte (tokens e GRs em BNF)
        LeitorArquivo leitor;
        EspecificacaoLinguagem especificacao = leitor.lerArquivo(caminhoEntrada);

        std::cout << "Arquivo carregado com sucesso: " << caminhoEntrada << "\n";
        std::cout << " - Tokens literais encontrados: " << especificacao.tokensLiterais.size() << "\n";
        std::cout << " - Gramaticas Regulares (BNF):  " << especificacao.gramaticas.size() << "\n";

        ExibidorTabela exibidor;

        // Etapa 2: Construção do AFND unificado
        ConstrutorAFND construtorAFND;
        AutomatoFinito afnd = construtorAFND.construir(especificacao);
        exibidor.exibir("ETAPA 2: AUTOMATO FINITO NAO-DETERMINISTICO (AFND)", afnd);

        // Etapa 3: Determinização (Construção de Subconjuntos)
        Determinizador determinizador;
        AutomatoFinito afd = determinizador.determinizar(afnd);
        exibidor.exibir("ETAPA 3: AUTOMATO FINITO DETERMINISTICO (AFD)", afd);

    } catch (const std::exception& erro) {
        std::cerr << "\n[ERRO] " << erro.what() << "\n";
        return 1;
    }

    return 0;
}