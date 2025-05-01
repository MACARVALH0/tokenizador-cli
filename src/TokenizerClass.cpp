#include <tokenizer-cli/TokenizerClass.hpp>
#include <tokenizer-cli/ErrorMsg.hpp>

Tokenizer::Tokenizer(const std::string& line)
{
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
                    tokenizer_cli::ErrorMsg err;
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
        tokenizer_cli::ErrorMsg err;
        err << "(Coluna " << col(line_start, char_iterator) << ") Buffer não vazio ao final do tokenizador: " << buffer << "\n";
        throw std::runtime_error(err.get());
    }
}


Tokenizer::OpTokenType
Tokenizer::processInitialCase(std::string::const_iterator& it, std::string::const_iterator& end)
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


std::string
Tokenizer::processFlagCase(std::string::const_iterator& it, std::string::const_iterator& l_begin, std::string::const_iterator& l_end)
{
    const std::string::const_iterator start = it; // Armazena a posição inicial do iterador que percorre a string.

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
        tokenizer_cli::ErrorMsg err;
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
        tokenizer_cli::ErrorMsg err;
        err << "(Coluna " << col(l_begin, start) << ") Flag `" << buffer << "` inválida (apenas hífens, sem identificador).\n";
        throw std::runtime_error(err.get());
    }

    return buffer;
}


std::string
Tokenizer::processStringCase(const std::string& line, std::string::const_iterator& it)
{
    std::string::const_iterator line_start = line.begin();  // Iterador marcando o início da linha de comando.
    std::string::const_iterator line_end = line.end();      // Iterador marcando o final da linha de comando.

    std::string buffer; // Define o buffer da string.
    const std::string::const_iterator start = it; // Salva o iterador do ponto de partida da análise.

    // Passo 1: Acumula caracteres no buffer até encontrar o próximo caractere `"` ou chegar no fim da linha.
    while(it != line_end && *it != '\"')
    {
        buffer += *it;
        it++;
    }

    // Passo 2: Confere se a string está fechada ou se alcançou o fim da linha de leitura.
    if(it == line_end)
    {
        tokenizer_cli::ErrorMsg err; // Logger de erro.
        err << "(Coluna " << col(line_start, start) << ") Caractere `\"` de fechamento não encontrado (EOF).\n";
        throw std::runtime_error(err.get()); // Lança exceção.
    }

    // Passo 3: Confere se a string no buffer está vazia ou contém apenas espaços.
    const std::string trimmed_buffer = trim(buffer);
    if(trimmed_buffer.empty())
    {
        tokenizer_cli::ErrorMsg err; // Logger de erro.
        err << "(Coluna " << col(line_start, start) << ") Strings vazias não são aceitas.";
        throw std::runtime_error(err.get()); // Lança exceção.
    }

    ++it; // Avança para o caractere além da aspa.

    // Passo 4: Caso nenhuma exceção seja lançada na análise, retorna o buffer.
    return buffer;
}


std::vector<tokenizer_cli::Token>
Tokenizer::getTokens(){ return tokens; }