#ifndef AUTOMATO_FINITO_HPP
#define AUTOMATO_FINITO_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

// Representa um estado q ∈ K do autômato e seus metadados de reconhecimento.
struct Estado {
    int id;
    std::string nome;
    bool ehInicial = false;
    bool ehFinal = false;
    bool ehErro = false;

    // Lista dos tokens ou gramáticas que este estado aceita (caso seja final)
    std::vector<std::string> tokensReconhecidos;
};

// Representa a quíntupla formal M = (K, Σ, δ, q0, F).
class AutomatoFinito {
private:
    std::vector<char> alfabeto;     // Σ (mantém ordem de descoberta para exibição)
    std::vector<Estado> estados;    // K (conjunto finito de estados)
    int idEstadoInicial = -1;       // q0

    // Função de transição δ: (idEstadoOrigem, simbolo) -> conjunto de idEstadosDestino
    std::map<int, std::map<char, std::set<int>>> transicoes;

public:
    AutomatoFinito() = default;

    // Manipulação do alfabeto (Σ)
    void adicionarSimboloAlfabeto(char simbolo);
    const std::vector<char>& obterAlfabeto() const;

    // Manipulação de estados (K, q0, F)
    int criarEstado(const std::string& nome, bool ehInicial = false,
                    bool ehFinal = false, bool ehErro = false);
    void marcarComoFinal(int idEstado, const std::string& rotuloToken);

    const std::vector<Estado>& obterEstados() const;
    const Estado& obterEstado(int idEstado) const;
    int obterIdEstadoInicial() const;

    // Manipulação da função de transição (δ)
    void adicionarTransicao(int idOrigem, char simbolo, int idDestino);
    std::set<int> obterDestinos(int idOrigem, char simbolo) const;
    bool possuiTransicao(int idOrigem, char simbolo) const;
};

#endif