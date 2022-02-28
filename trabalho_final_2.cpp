//+------------------------------------------------------------------+
//|                                                   Trabalho Final |
//|              Alunos: Leonardo Barros Bilhalva  e Santiago Gonzaga|
//|                     Disciplina: Classificacao e Pesquisa de Dados|
//+------------------------------------------------------------------+

#include <bits/stdc++.h>
#include <sstream>
#include <windows.h>
#include <time.h>
#include "./parser.hpp"

using namespace aria::csv;
using namespace std;

// CONSTANTS
const int ALPHABET_SIZE = 91;
const int HASH_TABLE_PLAYER_SIZE = 3789;   // 18945 dividido por 5 = 3789 -> m = n/5 

// STRUCTS

struct Player {
    string sofifa_id;
    string name;
    vector<string> player_positions;
};

struct Rating {
    string user_id;
    string sofifa_id;
    float rating;
};

struct Tag {
    string user_id;
    string sofifa_id;
    string tag;
};

struct RatingVector {
    string user_id;
    vector<string> sofifa_id;
    vector<float>  rating;
};

struct PlayerRating {
    string sofifa_id;
    float rating;
};

struct UserRating {
    string user_id;
    float rating;
};

struct MatrizPosition {
    string position;
    vector<PlayerRating> player_and_rating;
};

struct PlayerUserRating {
    string sofifa_id;
    string name;
    float global_rating;
    int count;
    float rating;
};

// HASH TABLE STRUCS
struct HashTablePlayer {
    Player player;
    vector<string> tags;
    vector<UserRating> users;
    int count_of_ratings;
    float sum_of_ratings;

    bool ocupado = false;
    bool usado = false;
    HashTablePlayer* next;
};

// TRIE TREE PLAYERS STRUCTS
struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;
    Player player;
};

struct TrieNode *getNode(void) {
    struct TrieNode *pNode =  new TrieNode;
 
    pNode->isEndOfWord = false;
 
    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;
 
    return pNode;
}

// TRIE TREE PLAYERS STRUCTS
struct TrieNode2 {
    struct TrieNode2 *children[ALPHABET_SIZE];
    bool isEndOfWord;
    vector<string> players;
};

struct TrieNode2 *getNode2(void) {
    struct TrieNode2 *pNode =  new TrieNode2;
 
    pNode->isEndOfWord = false;
 
    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;
 
    return pNode;
}

// HASH TABLE FUNCTIONS

void make_hash_table_player(Player player, vector<HashTablePlayer *> &hashTable) { // Função que cria uma hash_table em hashTable a partir do vetor_de_nomes 
    long long hash = 0;                                 //somatorio para hash de cada nome

        hash = 0;
        for(int b=0; b< player.sofifa_id.size(); b++)   //percorre caracteres do nome atual
        { 
            hash = (11 * hash + player.sofifa_id[b]) % HASH_TABLE_PLAYER_SIZE;                // 11 é o número primo escolhido para potência da string, pois é o maior número primo após 10 | o módulo é feito pelo tamanho da tabela hash atual            
        }
        // temos um hash pronto para ser alocado em -> long long hash

        if(hashTable[hash] != NULL) // Se tabela hash apontar para diferente de nulo, significa que já foi preenchido
        {
            HashTablePlayer *pointer = hashTable[hash];                         //cria um ponteiro para percorrer os proximos

            while(pointer->next != NULL)                                                //percorre até chegar no ponteiro que terá o próximo nulo
            {
                pointer = pointer->next;
            }

            HashTablePlayer* temp1 = new HashTablePlayer;                   // cria um ponteiro temporário e aloca os dados de forma igual a alocação base
            temp1->player.sofifa_id       = player.sofifa_id;
            temp1->player.name           = player.name;
            temp1->player.player_positions = player.player_positions;   
            temp1->count_of_ratings = 0;
            temp1->sum_of_ratings = 0;
            temp1->users = {};         
            temp1->ocupado = true;
            temp1->usado   = true;
            temp1->next    = NULL;

            pointer->next = temp1;                                                      // o ponteiro do último elemento da lista obtido no while é utilizado para indicar o novo próximo elemento, que é o alocado a cima em temp1
        }
        else    //caso aponta para nulo -> preencher!
        {   
            hashTable[hash] = new HashTablePlayer;          // aloca uma nova estrutura e preenche
            hashTable[hash]->player.sofifa_id       = player.sofifa_id;
            hashTable[hash]->player.name            = player.name;
            hashTable[hash]->player.player_positions = player.player_positions;  
            hashTable[hash]->count_of_ratings = 0;
            hashTable[hash]->sum_of_ratings = 0;
            hashTable[hash]->users = {}; 
            hashTable[hash]->ocupado = true;
            hashTable[hash]->usado   = true;
            hashTable[hash]->next    = NULL;
        }    
}

void add_hash_table_ratings(vector<HashTablePlayer *> &hashTable, Rating rating)  // Função que pesquisa um nome em uma tabela hash e retorna a quantidade de buscas/colisões da pesquisa
{   
    long long hash = 0;                                 //somatorio para hash de cada nome

        hash = 0;
        for(int b=0; b< rating.sofifa_id.size(); b++)   //percorre caracteres do nome atual
        { 
           hash = (11 * hash + rating.sofifa_id[b]) % HASH_TABLE_PLAYER_SIZE;                // 59 é o número primo escolhido para potência da string, pois é o maior número primo após 53 | o módulo é feito pelo tamanho da tabela hash atual            
        }
        // temos um hash pronto para ser alocado em -> long long hash

        if(hashTable[hash] != NULL) // Se tabela hash apontar para diferente de nulo, significa que já foi preenchido
        {
            HashTablePlayer *pointer = hashTable[hash];                         //cria um ponteiro para percorrer os proximos

            while(pointer->next != NULL && pointer->player.sofifa_id != rating.sofifa_id)         //percorre até chegar no ponteiro que terá ou o próximo nulo ou o nome atual
            {
                pointer = pointer->next;
            }

            if(pointer->player.sofifa_id == rating.sofifa_id) 
            {
              pointer->count_of_ratings ++;         //adiciona um novo ratings
              pointer->sum_of_ratings += rating.rating;
              pointer->users.push_back({rating.user_id, rating.rating});
            }         
        }
}

 void add_hash_table_tags(vector<HashTablePlayer *> &hashTable, string sofifa_id, string tag) // Função que pesquisa um nome em uma tabela hash e retorna a quantidade de buscas/colisões da pesquisa
 {   
    long long hash = 0;                                 //somatorio para hash de cada nome

        hash = 0;
        for(int b=0; b< sofifa_id.size(); b++)   //percorre caracteres do nome atual
        { 
           hash = (11 * hash + sofifa_id[b]) % HASH_TABLE_PLAYER_SIZE;                // 59 é o número primo escolhido para potência da string, pois é o maior número primo após 53 | o módulo é feito pelo tamanho da tabela hash atual            
        }
        // temos um hash pronto para ser alocado em -> long long hash

        if(hashTable[hash] != NULL) // Se tabela hash apontar para diferente de nulo, significa que já foi preenchido
        {
            HashTablePlayer *pointer = hashTable[hash];                         //cria um ponteiro para percorrer os proximos

            while(pointer->next != NULL && pointer->player.sofifa_id != sofifa_id)         //percorre até chegar no ponteiro que terá ou o próximo nulo ou o nome atual
            {
                pointer = pointer->next;
            }

            if(pointer->player.sofifa_id == sofifa_id) 
            {
              pointer->tags.push_back(tag);    //adicionando nova tag
            }

        }
}

HashTablePlayer *search_hash_table(vector<HashTablePlayer *> &hashTable, string sofifa_id)  // Função que pesquisa um nome em uma tabela hash e retorna a quantidade de buscas/colisões da pesquisa
{   
    long long hash = 0;                                 //somatorio para hash de cada nome

        hash = 0;
        for(int b=0; b< sofifa_id.size(); b++)   //percorre caracteres do nome atual
        { 
           hash = (11 * hash + sofifa_id[b]) % HASH_TABLE_PLAYER_SIZE;                // 59 é o número primo escolhido para potência da string, pois é o maior número primo após 53 | o módulo é feito pelo tamanho da tabela hash atual            
        }
        // temos um hash pronto para ser alocado em -> long long hash

        if(hashTable[hash] != NULL) // Se tabela hash apontar para diferente de nulo, significa que já foi preenchido
        {
            HashTablePlayer *pointer = hashTable[hash];                         //cria um ponteiro para percorrer os proximos

            while(pointer->next != NULL && pointer->player.sofifa_id != sofifa_id)         //percorre até chegar no ponteiro que terá ou o próximo nulo ou o nome atual
            {
                pointer = pointer->next;
            }

            if(pointer->player.sofifa_id == sofifa_id) 
            {
              return pointer;
            }     

        }
}

// TRIE TREE PLAYERS FUNCTIONS

void insertTrieNode(struct TrieNode *root, Player player) {
    struct TrieNode *pCrawl = root;
 
    for (int i = 0; i < player.name.length(); i++) {
        int index = player.name[i] - ' ';
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
 
        pCrawl = pCrawl->children[index];
    }
  
    pCrawl->isEndOfWord = true;
    pCrawl->player.name = player.name;
    pCrawl->player.player_positions = player.player_positions;
    pCrawl->player.sofifa_id = player.sofifa_id;
}

void searchAllWordsByPrefix(struct TrieNode *root, vector<HashTablePlayer *> hashTable) {

    if (root->isEndOfWord) {
        HashTablePlayer* playerCompleted = search_hash_table(hashTable, root->player.sofifa_id);
        cout << root->player.sofifa_id << " | " << root->player.name << " | ";
        for(int c =0; c < playerCompleted->player.player_positions.size(); c++) {
            cout << playerCompleted->player.player_positions[c] << ", ";
            if(c+1 > playerCompleted->player.player_positions.size())
                cout << playerCompleted->player.player_positions[c];
        }
        cout << " | " << playerCompleted->sum_of_ratings / playerCompleted->count_of_ratings << " | " << playerCompleted->count_of_ratings << endl;
    }

    for(int i = 0; i < ALPHABET_SIZE; i++) {
        if(root->children[i] != NULL) {          
            searchAllWordsByPrefix(root->children[i], hashTable);
        }
    }
}

bool searchTrieNode(struct TrieNode *root, string key, vector<HashTablePlayer *> hashTable) {
    struct TrieNode *pCrawl = root;
 
    for (int i = 0; i < key.length(); i++) {
        int index = key[i] - ' ';
        if (!pCrawl->children[index])
            return false;
 
        pCrawl = pCrawl->children[index];
    }
 
    searchAllWordsByPrefix(pCrawl, hashTable);
    return (pCrawl->isEndOfWord);
}

// TRIE TREE TAGS FUNCTIONS

void insertTrieNode2(struct TrieNode2 *root, string sofifa_id, string tag) {
    struct TrieNode2 *pCrawl = root;
 
    for (int i = 0; i < tag.length(); i++) {
        int index = tag[i] - ' ';
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode2();
 
        pCrawl = pCrawl->children[index];
    }
  
    pCrawl->isEndOfWord = true;
    pCrawl->players.push_back(sofifa_id);
}

vector<string> searchTrieNode2(struct TrieNode2 *root, string key) {
    struct TrieNode2 *pCrawl = root;
 
    for (int i = 0; i < key.length(); i++) {
        int index = key[i] - ' ';
        if (!pCrawl->children[index]) {
            vector<string> emptyVector;
            return emptyVector;
        }
        pCrawl = pCrawl->children[index];
    }
 
    return (pCrawl->players);
}

vector<string> intersection(vector<string> &v1, vector<string> &v2) {
    vector<string> v3;

    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());

    set_intersection(v1.begin(),v1.end(), v2.begin(),v2.end(), back_inserter(v3));
    return v3;
}

// funcao 4

void filterPlayersByTags(vector<string> players, vector<string> tags, vector<HashTablePlayer *> hashTable) {
    vector<string> filteredPlayers;
    vector<string> inter;

    for(int k=0; k < players.size(); k++) {
        HashTablePlayer* playerCompleted = search_hash_table(hashTable,players[k]);
        vector<string> tagsPlayer = playerCompleted->tags;

        auto inter = intersection(tags, tagsPlayer);

        if (inter.size() == tags.size())
            filteredPlayers.push_back(players[k]);
    }

    for(int i=0; i < filteredPlayers.size(); i++) {
        HashTablePlayer* playerCompleted = search_hash_table(hashTable,filteredPlayers[i]);
        cout << playerCompleted->player.sofifa_id << " | " << playerCompleted->player.name << " | ";
        for(int c =0; c < playerCompleted->player.player_positions.size(); c++) {
            cout << playerCompleted->player.player_positions[c] << ", ";
            if(c+1 > playerCompleted->player.player_positions.size())
                cout << playerCompleted->player.player_positions[c];
        }
        cout << " | " << playerCompleted->sum_of_ratings / playerCompleted->count_of_ratings << " | " << playerCompleted->count_of_ratings << endl;
    }
}

// func 3

void match_position_and_sofifa_to_vector(vector<MatrizPosition> &matriz_posicoes, string word, string sofifa_id)
{
    for (int i = 0; i < matriz_posicoes.size(); i++)
    {
        if (i + 1 == matriz_posicoes.size() && word != matriz_posicoes[i].position)
        {
            matriz_posicoes.push_back({word, {{sofifa_id, 0}}}); // cria nova position
            break;
        }
        if (matriz_posicoes[i].position == word)
        {
            matriz_posicoes[i].player_and_rating.push_back({sofifa_id, 0}); //adiciona a position jÃ¡ existente
            break;
        }
    }
}

void match_rating_to_vector(vector<MatrizPosition> &matriz_posicoes, vector<HashTablePlayer *> hashTable) //adiciona todas ratings a todos players
{
    HashTablePlayer *player;
    for (int a = 0; a < matriz_posicoes.size(); a++)
    {
        for (int b = 0; b < matriz_posicoes[a].player_and_rating.size(); b++)
        {
            player = search_hash_table(hashTable, matriz_posicoes[a].player_and_rating[b].sofifa_id);
            if (player->count_of_ratings >= 1000)
            {
                matriz_posicoes[a].player_and_rating[b].rating = player->sum_of_ratings / (double)player->count_of_ratings; // ou conta o rating
            }
            else
            {
                matriz_posicoes[a].player_and_rating.erase(matriz_posicoes[a].player_and_rating.begin() + b, matriz_posicoes[a].player_and_rating.begin() + b + 1); // ou apaga o que tem menos de 1000 da lista
            }
        }
    }
}

bool compare_by_rating(const PlayerRating &a, const PlayerRating &b)
{
    return a.rating > b.rating;
}

void order_rating_vector(vector<MatrizPosition> &matriz_posicoes)
{
    for (int a = 0; a < matriz_posicoes.size(); a++)
    {
        sort(matriz_posicoes[a].player_and_rating.begin(), matriz_posicoes[a].player_and_rating.end(), compare_by_rating);
    }
}

void search_top_player(vector<MatrizPosition> matriz_posicoes, string position, int total, vector<HashTablePlayer *> hashTable)
{
    HashTablePlayer *player;

    for (int a = 0; a < matriz_posicoes.size(); a++)
    {
        if (matriz_posicoes[a].position == position)
        {
            for (int b = 0; b < total; b++)
            {
                player = search_hash_table(hashTable, matriz_posicoes[a].player_and_rating[b].sofifa_id);
                cout << player->player.sofifa_id << " | " << player->player.name << " | ";
                for(int c =0; c < player->player.player_positions.size(); c++)
                {
                    cout << player->player.player_positions[c] << ", ";
                    if(c+1 > player->player.player_positions.size())
                        cout << player->player.player_positions[c] << " | ";
                }
                cout << matriz_posicoes[a].player_and_rating[b].rating << " | " << player->count_of_ratings << endl;
            }

            break;
        }
    }
}

// funcao 2

bool compare_by_rating2(const PlayerUserRating &a, const PlayerUserRating &b) {
    return a.rating > b.rating;
}

void searchRatingsByUser(vector<HashTablePlayer *> hashTable, string user_id, vector<string> players) {
    vector <PlayerUserRating> playerRatedByUser;

    for (int i=0; i < players.size(); i++) {
        HashTablePlayer* playerCompleted = search_hash_table(hashTable,players[i]);
        for(int j=0; j < playerCompleted->users.size(); j++) {
            if (playerCompleted->users[j].user_id == user_id) {
                playerRatedByUser.push_back({playerCompleted->player.sofifa_id, playerCompleted->player.name, playerCompleted->sum_of_ratings / playerCompleted->count_of_ratings, playerCompleted->count_of_ratings, playerCompleted->users[j].rating});
            }
        }
    }

    sort(playerRatedByUser.begin(), playerRatedByUser.end(), compare_by_rating2);

    for(int k=0; k < playerRatedByUser.size()  && k < 20; k++) {
        cout << playerRatedByUser[k].sofifa_id << " | " << playerRatedByUser[k].name << " | " << playerRatedByUser[k].global_rating << " | " << playerRatedByUser[k].count << " | " << playerRatedByUser[k].rating << endl;
    }
}

//MAIN

int main() {
    struct TrieNode *rootPlayers = getNode(); 
    struct TrieNode2 *rootTags = getNode2(); 
    vector<string> playersByTag;
    vector<string> positions;
    vector<string> players;
    vector<string> vetor_entrada;
    vector<HashTablePlayer *> hashTablePlayer(HASH_TABLE_PLAYER_SIZE);
    vector<MatrizPosition> matriz_posicoes(1);
    string word;
    string entrada;

    cout << "TRABALHO FINAL LEONARDO E SANTIAGO" << endl << "INICIANDO PRE-PROCESSAMENTO" << endl << endl;

    ifstream f("players.csv");
    CsvParser parser(f);

    for (auto& row : parser){

        stringstream ss(row[2]);

        positions.clear();
        while(ss >> word) {
            word.erase(remove(word.begin(), word.end(), ','), word.end()); //limpa ,
            word.erase(remove(word.begin(), word.end(), ' '), word.end()); //limpa ' ' das posições dos jogadores
            match_position_and_sofifa_to_vector(matriz_posicoes, word, row[0]); // word -> player position | row[0] -> sofifa_id
            positions.push_back(word);
        }

        players.push_back(row[0]);
        insertTrieNode(rootPlayers, {row[0],row[1],positions});
        make_hash_table_player({row[0],row[1],positions}, hashTablePlayer);
    }

    f.close();
    f.open("rating.csv");
    CsvParser parser2(f);
    for (auto& row : parser2) {
        add_hash_table_ratings(hashTablePlayer, {row[0], row[1], (float)atof(row[2].c_str())});       //adiciona o rating(global) na hashtable player 
    }

    match_rating_to_vector(matriz_posicoes, hashTablePlayer); //aloca os ratings no vetor matriz_posicoes
    order_rating_vector(matriz_posicoes);

    f.close();
    f.open("tags.csv");
    CsvParser parser3(f);

    for (auto& row : parser3) {
        insertTrieNode2(rootTags, row[1], row[2]);
        add_hash_table_tags(hashTablePlayer, row[1], row[2]);
    }

    cout << "PRE PROCESSAMENTO TERMINOU" << " tempo duracao: " << endl << "O programa comecou." << endl;

    do
    {
        // limpar a tela
        cout << "Lista de funcoes: " << endl;
        cout << "Funcao 'player' -> pesquisa sobre os nomes de jogadores" << endl;
        cout << "Funcao 'user' -> pesquisa sobre jogadores revisados por usuarios" << endl;
        cout << "Funcao 'topN' -> pesquisa sobre os melhores jogadores de uma determinada posicao" << endl;
        cout << "Funcao 'tags' -> pesquisa sobre 'tags' de jogadores" << endl << endl;

        cout << "Entre 0 para terminar o programa ou entre a funcao desejada: " << endl;
        fflush(stdin);
        getline(cin,entrada);

        stringstream ss(entrada);
        vetor_entrada.clear();

        while(ss >> word)
        {
          vetor_entrada.push_back(word);
        }

        if(vetor_entrada[0] == "player")
        {
            // f1
            entrada.erase(entrada.begin(), entrada.begin() + 6);    // apaguei da entrada "player" para sobrar so o nome
            entrada.erase(remove(entrada.begin(), entrada.end(), ' '), entrada.end()); // tira o espaço

           if(!entrada.empty());
            searchTrieNode(rootPlayers, entrada, hashTablePlayer);

            cout << "A funcao terminou. Entre algo para voltar ao menu principal: ";
            fflush(stdin);
            getchar();
        }
        else if(vetor_entrada[0] == "user")
        { 
            searchRatingsByUser(hashTablePlayer, vetor_entrada[1], players);
            cout << "A funcao terminou. Entre algo para voltar ao menu principal: ";
            fflush(stdin);
            getchar();
        }
        else if(vetor_entrada[0][0] == 't' && vetor_entrada[0][1] == 'o' && vetor_entrada[0][2] == 'p')         //função 3 do programa
        {
            vetor_entrada[0].erase(vetor_entrada[0].begin(), vetor_entrada[0].begin()+3);// apaga o top

            vetor_entrada[1].erase(remove(vetor_entrada[1].begin(), vetor_entrada[1].end(), '\''), vetor_entrada[1].end());  //removendo apostrofe
            vetor_entrada[1].erase(remove(vetor_entrada[1].begin(), vetor_entrada[1].end(), '\"'), vetor_entrada[1].end());  //removendo aspas

            search_top_player(matriz_posicoes, vetor_entrada[1], stoi(vetor_entrada[0]), hashTablePlayer);
            cout << "A funcao terminou. Entre algo para voltar ao menu principal: ";
            fflush(stdin);
            getchar();
        }
        else if(vetor_entrada[0] == "tags")
        {
            vetor_entrada.erase(vetor_entrada.begin(), vetor_entrada.begin() + 1);

            for(int a = 0; a< vetor_entrada.size(); a++)
            {
              vetor_entrada[a].erase(remove(vetor_entrada[a].begin(), vetor_entrada[a].end(), '\''), vetor_entrada[a].end());  //removendo apostrofe
              vetor_entrada[a].erase(remove(vetor_entrada[a].begin(), vetor_entrada[a].end(), '\"'), vetor_entrada[a].end());  //removendo aspas
            }
            playersByTag =  searchTrieNode2(rootTags, vetor_entrada[0]);

            vetor_entrada.erase(vetor_entrada.begin(), vetor_entrada.begin() + 1);

            sort( playersByTag.begin(), playersByTag.end());
            playersByTag.erase( unique( playersByTag.begin(), playersByTag.end() ), playersByTag.end()); // transofrmando em um set
            filterPlayersByTags(playersByTag, vetor_entrada, hashTablePlayer);

            cout << "A funcao terminou. Entre algo para voltar ao menu principal: ";
            fflush(stdin);
            getchar();
        }
        else if(vetor_entrada[0] != "0")
        {
            // f4 search
            cout << "A funcao entrada foi invalida. Por favor, tente novamente inserindo qualquer tecla para voltar ao menu.";
            fflush(stdin);
            getchar();
        }
    
    system("CLS");
    }while(vetor_entrada[0] != "0");

    cout << "O programa terminou!!!" << endl;

    return 0;
}
