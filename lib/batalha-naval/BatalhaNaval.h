#ifndef _BATALHANAVAL_H_
#define _BATALHANAVAL_H_

#include <vector>
#include <string>

using namespace std;

class BatalhaNaval
{
private:
    const int tsize = 10;
    char tabuleiro[10][10];
    char tabuleiro_adversario[10][10];
    char embarcacoes[4] = {'P', 'C', 'D', 'S'};
    int num_embarcacoes[4] = {5, 4, 3, 2};

    bool verifica(int linha, int coluna, int casas, char eixo);
    int num_casas(char embacacao);

public:
    BatalhaNaval();

    vector<vector<char>> endereco;
    int pontos = 0;

    bool inserir(char tipo_embacacao, char eixo, char linha, int coluna);
    void insere_endereco(char linha, int coluna);
    bool complet_aleatorio();
    bool complet_file(string arquivo);

    char pontuacao(char linha, int coluna);
    char jogada(char linha, char coluna);
    void jogada_adversaria(char *dado);
    bool afundou(int indice);

    void print_tabuleiro();
    int converte_linha(char linha);
    char converte_linha(int linha);
    char converte_coluna(int coluna);
};

#endif