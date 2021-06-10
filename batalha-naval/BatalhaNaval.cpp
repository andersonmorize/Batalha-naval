#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "BatalhaNaval.h"

using namespace std;

BatalhaNaval::BatalhaNaval()
{
    // zerando matriz
    for (int i = 0; i < tsize; i++)
    {
        for (int j = 0; j < tsize; j++)
        {
            tabuleiro[i][j] = '-';
            tabuleiro_adversario[i][j] = '-';
        }
    }
}

int BatalhaNaval::num_casas(char embarcacao)
{
    for (int i = 0; i < 4; i++)
        if (embarcacoes[i] == embarcacao)
            return num_embarcacoes[i];

    return 0;
}

// verifica se pode introduzir a embarcação nesta posição
bool BatalhaNaval::verifica(int linha, int coluna, int casas, char eixo)
{

    if (linha + casas < BatalhaNaval::tsize && linha + casas >= 0 && (eixo == 'v' || eixo == 'V'))
    {
        for (int i = 0; i < casas; i++)
        {
            if (BatalhaNaval::tabuleiro[linha + i][coluna] != '-')
                return false;
        }
        return true;
    }
    else if (coluna + casas < BatalhaNaval::tsize && coluna + casas >= 0 && (eixo == 'h' || eixo == 'H'))
    {
        for (int i = 0; i < casas; i++)
        {
            if (BatalhaNaval::tabuleiro[linha][coluna + i] != '-')
                return false;
        }
        return true;
    }
    else
        return false;
}

// introduz a embarcação no tabuleiro
bool BatalhaNaval::inserir(char tipo_embarcacao, char eixo, char linha, int coluna)
{
    int casas = num_casas(tipo_embarcacao);
    int linhai = converte_linha(linha);

    if ((eixo == 'v' || eixo == 'V') && verifica(linhai, coluna, casas, eixo))
    {
        vector<char> em;
        em.push_back(tipo_embarcacao);
        for (int i = 0; i < casas; i++)
        {
            em.push_back(converte_linha((linhai + 1) + i));
            em.push_back(converte_coluna(coluna));
            tabuleiro[linhai + i][coluna] = tipo_embarcacao;
        }
        endereco.push_back(em);

        //endereco(tipo_embarcacao, eixo, linha, coluna);
        return true;
    }
    else if ((eixo == 'h' || eixo == 'H') && verifica(linhai, coluna, casas, eixo))
    {
        vector<char> em;
        em.push_back(tipo_embarcacao);
        for (int i = 0; i < casas; i++)
        {
            em.push_back(converte_linha(linhai + 1));
            em.push_back(converte_coluna(coluna + i));
            tabuleiro[linhai][coluna + i] = tipo_embarcacao;
        }
        endereco.push_back(em);

        //endereco(tipo_embarcacao, eixo, linha, coluna);
        return true;
    }
    else
        return false;
}

// completa o tabuleiro aleatoriamente
// metodo pesado
bool BatalhaNaval::complet_aleatorio()
{
    char em[10] = {'P', 'C', 'C', 'D', 'D', 'D', 'S', 'S', 'S', 'S'};

    try
    {
        int linha_random, coluna_random;

        for (int i = 0; i < 11; i++)
        {
            srand((unsigned)time(0));
            int eixo = (rand() % 2);

            if (eixo)
            {
                do
                {

                    srand((unsigned)time(0));
                    linha_random = (rand() % 10);
                    coluna_random = (rand() % 10);

                } while (!inserir(em[i], 'V', converte_linha(linha_random), coluna_random));
            }
            else
            {
                do
                {

                    srand((unsigned)time(0));
                    linha_random = (rand() % 10);
                    coluna_random = (rand() % 10);

                } while (!inserir(em[i], 'H', converte_linha(linha_random), coluna_random));
            }
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool BatalhaNaval::complet_file(string arquivo)
{
    ifstream file(arquivo);
    unsigned size = 0;

    if (!file)
    {
        //erro durante leitura
        return false;
    }

    vector<char> matrix;
    //matrix.reserve(size*size);

    while (true)
    {
        char value;
        if (!(file >> value))
        {
            break;
        }
        matrix.push_back(value);
    }

    for (int i = 0; i < matrix.size(); i++)
    {
        char tipo = matrix[i++];
        char eixo = matrix[i++];
        char linha = matrix[i++];
        int coluna = (int)matrix[i] - '0';

        if (!inserir(tipo, eixo, linha, coluna))
            return false;
    }

    print_tabuleiro();

    return true;
}

char BatalhaNaval::jogada(char linha, char coluna)
{
    int linhai = converte_linha(linha);
    int colunai = (int)coluna - '0';

    if (linhai < 0 || linhai > 10 || colunai < 0 || colunai > 10)
    {
        return '?';
    }
    else if (tabuleiro[linhai][colunai] != '-' &&
             tabuleiro[linhai][colunai] != 'x')
    {
        for (int i = 0; i < endereco.size(); i++)
        {
            for (int j = 1; j < endereco[i].size(); j++)
            {
                if (endereco[i][j] == linha && endereco[i][j + 1] == coluna)
                {
                    endereco[i][j] = '-';
                    endereco[i][++j] = '-';
                    
                    if(afundou(i))
                    {
                        return endereco[i][0];
                    }
                    else
                    {
                        return '#';
                    }
                }
            }
        }
    }
    else if (tabuleiro[linhai][colunai] == '-')
    {
        tabuleiro[linhai][colunai] = 'x';
        return 'x';
    }
}

bool BatalhaNaval::afundou(int indice)
{
    for(int i = 1; i < endereco[indice].size(); i++)
    {
        if(endereco[indice][i] != '-')
        {
            return false;
        }
    }
    return true;
}

void BatalhaNaval::jogada_adversaria(char *dado)
{
    int linha = converte_linha(dado[0]);
    int coluna = (int)dado[1] - '0';

    if(dado[2] != 'x' && dado[2] != '#' && dado[2] !='?')
    {
        pontos += num_casas(dado[2]);
    }

    tabuleiro_adversario[linha][coluna] = dado[2];
}

void BatalhaNaval::print_tabuleiro()
{

    cout << "       Meus navios"
         << "           Navios adversarios" << endl;

    cout << "  ";
    for (int j = 0; j < tsize; j++)
    {
        cout << j + 1 << " ";
    }

    cout << "     ";
    for (int j = 0; j < tsize; j++)
    {
        cout << j + 1 << " ";
    }

    cout << '\n';

    char linha[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

    for (int i = 0; i < tsize; i++)
    {
        cout << linha[i] << " ";
        for (int j = 0; j < tsize; j++)
        {
            cout << tabuleiro[i][j] << " ";
        }
        cout << linha[i] << " ";

        cout << "  " << linha[i] << " ";
        for (int j = 0; j < tsize; j++)
        {
            cout << tabuleiro_adversario[i][j] << " ";
        }
        cout << linha[i] << " ";

        cout << '\n';
    }

    cout << "  ";
    for (int j = 0; j < tsize; j++)
        cout << j + 1 << " ";

    cout << "     ";
    for (int j = 0; j < tsize; j++)
    {
        cout << j + 1 << " ";
    }
    cout << endl;
}

int BatalhaNaval::converte_linha(char linha)
{
    char aux[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

    for (int i = 0; i < 10; i++)
        if (linha == aux[i])
            return i;
}

char BatalhaNaval::converte_linha(int linha)
{
    char aux[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

    return aux[linha - 1];
}

char BatalhaNaval::converte_coluna(int coluna)
{
    char aux[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    return aux[coluna];
}