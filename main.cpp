#include <iostream>

#include "batalha-naval/BatalhaNaval.h"
#include "mysocket/mysocket.h"

using namespace std;

int main()
{
    BatalhaNaval b;

    // posicionamento das embarcaçoes
    cout << "******* BATALHA NAVAL *******\n";

    cout << "\n COLOCAR OS NAVIOS\n";
    cout << "1 - Aletoriamente\n";
    cout << "2 - Manualmente\n";
    cout << "3 - Via Arquivo\n";
    cout << ": ";
    int coloca_embarcacao;
    cin >> coloca_embarcacao;

    // para posicionar aleatoriamente
    if (coloca_embarcacao == 1)
    {
        if (b.complet_aleatorio())
        {
            b.print_tabuleiro();
        }
        else
        {
            cout << "Erro ao introduzir embarcações\n";
            cout << "Tente novamente\n";
        }
    }

    // para posicionar manualmente
    else if (coloca_embarcacao == 2)
    {
        char linha, eixo, tipo;
        int coluna;

        cout << "\nIntroduzir embarcações\n";
        for (int cont = 0; cont < 11; cont++)
        {
            cout << "Informe a linha, coluna, embarcação e eixo(horizontal/vertical)\n"
                 << "onde se deseja posicionar a embarcação\n";
            cout << "ex: P V D 2\n";
            cout << ": ";

            cin >> tipo >> eixo >> linha >> coluna;

            if (b.inserir(tipo, eixo, linha, coluna - 1))
                cout << "Embarcação inserida com sucesso\n";
            else
                cout << "Erro ao introduzir embarcação\n";

            b.print_tabuleiro();
        }
    }

    // posicionar lendo coordenadas em um txt
    else if (coloca_embarcacao == 3)
    {
        cout << "Informe o endereço para o arquivo" << endl;

        string arquivo;

        cin >> arquivo;

        if (b.complet_file(arquivo))
        {
            cout << "Arquivo lido com sucesso!\n";
        }
        else
        {
            cout << "Falha ao ler o arquivo!\n";
        }
    }

    cout << "******* Definição cliente/servidor *******\n";

    cout << "1 - Cliente\n";
    cout << "2 - Servidor\n";
    cout << ": ";

    int opcao;

    cin >> opcao;

    tcp_mysocket_server server;
    tcp_mysocket client;

    char porta[6];
    char ip[100];

    // opção cliente
    if (opcao == 1)
    {
        client.init();
        cout << "Informe o ip: ";
        cin >> ip;
        cout << "Informe a porta: ";
        cin >> porta;

        if (client.connect(ip, porta) != MYSOCKET_OK)
        {
            cout << "Erro ao se conectar!" << endl;
        }

        cout << "Modo de jogo (manual/automatico)" << endl;
        cout << "M - Manual" << endl;
        cout << "A - Automatico" << endl;
        cout << ": ";

        char modo;
        cin >> modo;

        cout << "\n\n\n\n\n\n\n";
        cout << "****** TABULEIRO ******" << endl;

        // opção manual
        if (modo == 'M')
        {
            char linha;
            int coluna;

            // formato para o envio
            char enviar[3];
            char receber[3];

            int cont = 0;

            while (true)
            {
                b.print_tabuleiro();

                // recebe jogada do adversario
                if(client.read(receber, 2) != MYSOCKET_OK)
                {
                    client.close();
                    break;
                }

                // faz jogar do adversario no seu tabuleiro
                enviar[0] = receber[0];
                enviar[1] = receber[1];
                enviar[2] = b.jogada(receber[0], receber[1]);

                // mandando resultado da jogada
                if(client.write(enviar, 3) != MYSOCKET_OK)
                {
                    client.close();
                    break;
                }

                // pede as posições a serem jogadas
                cout << endl;
                cout << ": ";
                cin >> linha >> coluna;

                // formato para o envio
                enviar[0] = linha;
                // converte o int para char
                // -1 para se encaixar no padrão 0-9
                enviar[1] = (coluna - 1) + '0';

                // envia jogada para o adversario
                if(client.write(enviar, 2) != MYSOCKET_OK)
                {
                    client.close();
                    break;
                }

                // recebe resultado da jogada
                if(client.read(receber, 3) != MYSOCKET_OK)
                {
                    client.close();
                    break;
                }

                b.jogada_adversaria(receber);

                if (b.pontos == 30)
                {
                    client.close();
                    client.end();
                    break;
                }
            }
        }
        // modo automatico do client
        else if (modo == 'A')
        {
            char enviar[3];
            char receber[3];

            int cont = 0;

            // responsavel pelas jogadas automaricas
            int linha = 0, coluna = 0;

            while (b.pontos < 30)
            {
                b.print_tabuleiro();

                // recebe jogada do adversario
                if(client.read(receber, 2) != MYSOCKET_OK)
                {
                    client.close();
                    client.end();
                    break;
                }

                // faz jogar do adversario no seu tabuleiro
                enviar[0] = receber[0];
                enviar[1] = receber[1];
                enviar[2] = b.jogada(receber[0], receber[1]);

                // mandando resultado da jogada
                if(client.write(enviar, 3) != MYSOCKET_OK)
                {
                    client.close();
                    client.end();
                    break;
                }

                // responsavel pelas jogadas automaricas
                if (linha < 10 && coluna < 10)
                {
                    // aproveita o char dado para mandar a jogada
                    // converte o valor de linha para A-J
                    // adiciona +1 para rodar na função converte
                    enviar[0] = b.converte_linha(linha + 1);
                    // converte para char de '0'-'9'
                    enviar[1] = coluna + '0';

                    // manda sua jogada para o adversario
                    if (client.write(enviar, 2) != MYSOCKET_OK)
                    {
                        client.close();
                        client.end();
                        break;
                    }

                    linha++;
                }

                if (linha == 10)
                {
                    linha = 0;
                    coluna++;
                }
                if (!client.accepting())
                {
                    client.close();
                    client.end();
                    break;
                }
                if (coluna == 10)
                {
                    client.close();
                    client.end();
                    break;
                }

                // recebe resultado da jogada
                if (client.read(receber, 3) != MYSOCKET_OK)
                {
                    client.close();
                    break;
                }

                b.jogada_adversaria(receber);

                cont++;
            }
        }
    }
    // modo servidor
    else if (opcao == 2)
    {
        server.init();
        cout << "Informe a porta: ";
        cin >> porta;

        if (server.listen("5000", 1) != MYSOCKET_OK)
        {
            cout << "Erro ao criar um novo socket para esperar conexoes" << endl;
        }

        cout << "Modo de jogo (manual/automatico)" << endl;
        cout << "M - Manual" << endl;
        cout << "A - Automatico" << endl;
        cout << ": ";

        char modo;
        cin >> modo;

        tcp_mysocket conn;

        // modo manual
        if (modo == 'M')
        {
            // aceita conexão
            if (server.accept(conn) != MYSOCKET_OK)
            {
                char linha;
                int coluna;

                // formato para o envio
                char enviar[3];
                char receber[3];

                int cont = 0;

                while (true)
                {
                    b.print_tabuleiro();

                    // pede as posições a serem jogadas
                    cout << endl;
                    cout << ": ";
                    cin >> linha >> coluna;

                    // formato para o envio
                    enviar[0] = linha;

                    // converte o int para char
                    // -1 para se encaixar no padrão 0-9
                    enviar[1] = (coluna - 1) + '0';

                    // envia jogada para o adversario
                    if (conn.write(enviar, 2) != MYSOCKET_OK)
                    {
                        cout << "enviar 1" << endl;
                        conn.close();
                        conn.end();
                        break;
                    }

                    // recebe resultado da jogada
                    if (conn.read(receber, 3) != MYSOCKET_OK)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    // aplica essa jogada no tabuleiro adversario (exibição)
                    b.jogada_adversaria(receber);

                    // ler a jogada do adversario
                    if (conn.read(receber, 2) != MYSOCKET_OK)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    // faz a jogada do adversario em seu tabuleiro
                    enviar[0] = receber[0];
                    enviar[1] = receber[1];
                    enviar[2] = b.jogada(receber[0], receber[1]);

                    // enviado resultado da jogada
                    if (conn.write(enviar, 3) != MYSOCKET_OK)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    if (b.pontos == 30)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }
                }
            }
            else
            {
                cout << "Erro na criação do socket" << endl;
            }
        }
        // modo automatico
        else if (modo == 'A')
        {
            // aceita conexão
            if (server.accept(conn) == MYSOCKET_OK)
            {
                char enviar[3];
                char receber[3];

                int cont = 0;

                // responsavel pelas jogadas automaricas
                int linha = 0, coluna = 0;

                
                while (b.pontos < 30)
                {
                    b.print_tabuleiro();

                    // responsavel pelas jogadas automaricas
                    if (linha < 10 && coluna < 10)
                    {
                        // aproveita o char dado para mandar a jogada
                        // converte o valor de linha para A-J
                        // adiciona +1 para rodar na função converte
                        enviar[0] = b.converte_linha(linha + 1);
                        // converte para char de '0'-'9'
                        enviar[1] = coluna + '0';

                        // manda sua jogada para o adversario
                        if (conn.write(enviar, 2) != MYSOCKET_OK)
                        {
                            conn.close();
                            conn.end();
                            break;
                        }

                        linha++;
                    }

                    if (linha == 10)
                    {
                        linha = 0;
                        coluna++;
                    }
                    if (coluna == 10)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    // recebe resultado da jogada
                    if (conn.read(receber, 3) != MYSOCKET_OK)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    // aplica essa jogada no tabuleiro adversario (exibição)
                    b.jogada_adversaria(receber);

                    // ler a jogada do adversario
                    if (conn.read(receber, 2) != MYSOCKET_OK)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    // faz a jogada do adversario em seu tabuleiro
                    enviar[0] = receber[0];
                    enviar[1] = receber[1];
                    enviar[2] = b.jogada(receber[0], receber[1]);

                    // enviado resultado da jogada
                    if (conn.write(enviar, 3) != MYSOCKET_OK)
                    {
                        conn.close();
                        conn.end();
                        break;
                    }

                    cont++;
                }
            }
            else
            {
                cout << "Erro na criação do socket" << endl;
            }
        }
    }
    b.print_tabuleiro();
    
    if(b.pontos == 30)
    {
        cout << "Vencedor!" << endl;
    }
    else
    {
        cout << "Perdedor!" << endl;
    }
    
    cout << "Pontuação = " << b.pontos << endl;
}
