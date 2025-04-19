#include "tokenizer.hpp"

/**
 * @brief Verifica se um caractere é alfanumérico.
 * 
 * @param c Caractere a ser analisado.
 * @return true 
 * @return false 
 */
static bool bisalnum(char c)
{ return std::isalnum(static_cast<unsigned char>(c)) != 0; };

/**
 * @brief Encontra o índice da posição de um iterador de uma string.
 * 
 * @param line_start Iterador apontando para o início da string.
 * @param it Iterador do caractere em questão.
 * @return int
 */
static int col(const std::string::iterator& line_start, const std::string::iterator& it)
{ return std::distance(line_start, it); };

/**
 * @brief Limpa o buffer do autômato e retorna ao estado inicial.
 * 
 * @param buffer Referência para o buffer do autômato.
 * @param state Referência para a variável de estado do autômato.
 */
static void to_initial(std::string& buffer, OpTokenType& state)
{ buffer.clear(); state = OpTokenType::INITIAL; };

/**
 * @brief Remove espaços em branco no início e no fim de uma string.
 * 
 * @param text Referência para a string que terá os espaços removidos das extremidades.
 * @return std::string&
 */
static std::string& trim(std::string& text)
{
    size_t start = text.find_first_not_of(" \t\n\r\f\v");
    size_t end = text.find_last_not_of(" \t\n\r\f\v");

    if (start == std::string::npos || end == std::string::npos) text.clear();
    else text = text.substr(start, end - start + 1);
    
    return text;
}



/**
 * @brief Processa a situação de caso inicial/padrão do tokenizador.
 * 
 * @param it Iterador para a linha de comando.
 * @return O estado `OpTokenType` identificado no iterador.
 */
static const OpTokenType processInitialCase(std::string::iterator& it, std::string::iterator& end)
{
    // Ignora espaços em branco.
    while(it != end && *it == ' ') it++;
    if(it == end) return OpTokenType::INITIAL;

    // Se o caractere for alfanumérico, retorna o estado IDENTIFIER.
    if(bisalnum(*it)) return OpTokenType::IDENTIFIER;

    // Se o caractere for `-`, retorna o estado FLAG.
    if(*it == '-') return OpTokenType::FLAG;

    // Se o caractere for `"`, retorna o estado STRING.
    if(*it == '\"')
    {
        it++; // Pula o caractere que abre aspas.
        return OpTokenType::STRING;
    }

    // Caractere não reconhecido encontrado, retorna state UNKNOWN.
    return OpTokenType::UNKNOWN;
}


/**
 * @brief Processa uma flag a partir da linha do comando.
 * 
 * @param line A linha de comando completa (usada apenas para mensagens de erro).
 * @param it Iterador para a linha de comando. Deve apontar para um hífen (`-`).
 * @return A flag processada.
 * @throws std::runtime_error Caso haja algum erro no processamento da flag.
 */
static std::string processFlagCase(std::string::iterator& it, std::string::iterator& l_begin, std::string::iterator& l_end)
{
    const std::string::iterator start = it; // Armazena a posição inicial do iterador que percorre a string.

    const int max_dash_count = 2; // Número máximo de hífens permitidos.
    int dash_count = 0;           // Contador de hífens.
    std::string buffer;           // Buffer para armazenar a flag.

    // Passo 1: Processa os hífens iniciais.
    // Como o iterador já aponta para um hífen, não precisamos verificar se a linha está vazia.
    while (it != l_end && *it == '-')
    {
        buffer += '-';
        dash_count++;
        it++;
    }

    // Passo 2: Valida o número de hífens.
    // Se houver mais hífens do que o permitido, retorna uma string vazia para indicar erro.
    if (dash_count > max_dash_count)
    {
        // Flag inválida.
        ErrorMsg err;
        err << "(Coluna " << col(l_begin, start) << ") Flag `" << buffer << "` inválida.\n";
        throw std::runtime_error(err.get());
    }

    // Passo 3: Processa os caracteres alfanuméricos da flag.
    while (it != l_end && bisalnum(*it))
    {
        buffer += *it;
        it++;
    }

    // Passo 4: Retorna a flag processada.
    // Se não houver caracteres alfanuméricos após os hífens (apenas hífens, nenhum alfanumérico), a flag é inválida.
    if (buffer.size() == dash_count)
    {
        // Flag inválida (apenas hífens, sem identificador).
        ErrorMsg err;
        err << "(Coluna " << col(l_begin, start) << ") Flag `" << buffer << "` inválida (apenas hífens, sem identificador).\n";
        throw std::runtime_error(err.get());
    }

    return buffer;
}


/**
 * @brief Processa uma string a partir da linha do comando.
 * 
 * @param line A linha de comando completa (usada apenas para mensagens de erro).
 * @param it Iterador para a linha de comando. Deve apontar para um caractere de aspas (`"`).
 * @return A string processada.
 * @throws std::runtime_error Caso haja algum erro no processamento da string.
 */
static std::string processStringCase(std::string& line, std::string::iterator& it)
{
    std::string::iterator line_start = line.begin();
    std::string::iterator line_end = line.end();

    std::string buffer; // Define o buffer da string.
    const std::string::iterator start = it; // Salva o iterador do ponto de partida da análise.

    // Passo 1: Acumula caracteres no buffer até encontrar o próximo caractere `"` ou chegar no fim da linha.
    while(it != line_end && *it != '\"')
    {
        buffer += *it;
        it++;
    }

    // Passo 2: Confere se a string está fechada ou se alcançou o fim da linha de leitura.
    if(it == line_end)
    {
        ErrorMsg err; // Logger de erro.
        err << "(Coluna " << col(line_start, start) << ") Caractere `\"` de fechamento não encontrado (EOF).\n";
        throw std::runtime_error(err.get()); // Lança exceção.
    }

    // Passo 3: Confere se a string no buffer está vazia ou contém apenas espaços.
    const std::string trimmed_buffer = trim(buffer);
    if(trimmed_buffer.empty())
    {
        ErrorMsg err; // Logger de erro.
        err << "(Coluna " << col(line_start, start) << ") Strings vazias não são aceitas.";
        throw std::runtime_error(err.get()); // Lança exceção.
    }

    ++it; // Avança para o caractere além da aspa.

    // Passo 4: Caso nenhuma exceção seja lançada na análise, retorna o buffer.
    return buffer;
}


/**
 * @brief Função de tokenização da linha de comando.
 * 
 * @param line Uma std::string contendo a linha de comando recebida do usuário.
 * @return std::vector<Token>
 * @throws std::runtime_error
 */
std::vector<Token> tokenize(std::string& line)
{
    // Vetor que armazenará os tokens gerados.
    std::vector<Token> tokens{};

    OpTokenType state = OpTokenType::INITIAL; // Estado inicial da máquina de estados.
    std::string buffer{}; // Acumula caracteres para análise no autômato.

    // Laço principal da máquina de estados.
    auto line_start = line.begin(); // Armazena posição inicial.
    auto line_end = line.end();
    auto char_iterator = line.begin(); // Iterador que percorrerá a linha.
    while(char_iterator != line_end)
    {
        switch(state)
        {
            case OpTokenType::INITIAL:

                state = processInitialCase(char_iterator, line_end);

                if(state == OpTokenType::UNKNOWN)
                {
                    ErrorMsg err;
                    err << "(Coluna " << col(line_start, char_iterator) << ") Caractere `" << *char_iterator << "` não identificado.\n" ;
                    throw std::runtime_error(err.get());
                }

            break;


            case OpTokenType::IDENTIFIER:

                // Acumula os caracteres alfanuméricos no buffer.
                while(char_iterator != line.end() && bisalnum(*char_iterator))
                {
                    buffer += *char_iterator;
                    char_iterator++;
                }

                // Adiciona o token do tipo IDENTIFIER ao vetor de tokens.
                tokens.emplace_back(buffer, state, col(line_start, char_iterator));
                to_initial(buffer, state); // Retorna ao estado inicial.
                
            break;


            case OpTokenType::FLAG:
                buffer = processFlagCase(char_iterator, line_start, line_end);

                tokens.emplace_back(buffer, state, col(line_start, char_iterator));
                to_initial(buffer, state);
            break;


            case OpTokenType::STRING:
                buffer = processStringCase(line, char_iterator);

                tokens.emplace_back(buffer, state, col(line_start, char_iterator));
                to_initial(buffer, state);
            break;
        }
    }


    // Verifica se o buffer está vazio ao final da análise.
    if(!buffer.empty())
    {
        ErrorMsg err;
        err << "(Coluna " << col(line_start, char_iterator) << ") Buffer não vazio ao final do tokenizador: " << buffer << "\n";
        throw std::runtime_error(err.get());
    }

    return tokens;
}