#ifndef TOKENIZER_CLASS
#define TOKENIZER_CLASS

#include <tokenizer-cli/tokenizer-cli.hpp>
#include <string>
#include <vector>

class Tokenizer
{
    public:
        // @brief Vetor de tokens resultantes da operação.
        std::vector<tokenizer_cli::Token> tokens{};

        /**
         * @brief Enumerador para os diferentes tipos de token possíveis.
         */
        enum class OpTokenType
        {
            INITIAL,
            UNKNOWN,
            IDENTIFIER,
            FLAG,
            STRING
        };

        /**
         * @brief Verifica se um caractere é alfanumérico.
         * 
         * @param c Caractere a ser analisado.
         * @return true 
         * @return false 
         */
        bool bisalnum(char c);

        /**
         * @brief Encontra o índice da posição de um iterador de uma string.
         * 
         * @param line_start Iterador apontando para o início da string.
         * @param it Iterador do caractere em questão.
         * @return int
         */
        int col(const std::string::const_iterator& line_start, const std::string::const_iterator& it);

        /**
         * @brief Limpa o buffer do autômato e retorna ao estado inicial.
         * 
         * @param buffer Referência para o buffer do autômato.
         * @param state Referência para a variável de estado do autômato.
         */
        void to_initial(std::string& buffer, OpTokenType& state);

        /**
         * @brief Remove espaços em branco no início e no fim de uma string.
         * 
         * @param text Referência para a string que terá os espaços removidos das extremidades.
         * @return std::string&
         */
        std::string& trim(std::string& text);

    private:

        /**
         * @brief Processa a situação de caso inicial/padrão do tokenizador.
         * 
         * @param it Iterador para a linha de comando.
         * @return O estado `OpTokenType` identificado no iterador.
         */
        OpTokenType processInitialCase(std::string::const_iterator& it, std::string::const_iterator& end);


        /**
         * @brief Processa uma flag a partir da linha do comando.
         * 
         * @param line A linha de comando completa (usada apenas para mensagens de erro).
         * @param it Iterador para a linha de comando. Deve apontar para um hífen (`-`).
         * @return A flag processada.
         * @throws std::runtime_error Caso haja algum erro no processamento da flag.
         */
        std::string processFlagCase(std::string::const_iterator& it, std::string::const_iterator& l_begin, std::string::const_iterator& l_end);


        /**
         * @brief Processa uma string a partir da linha do comando.
         * 
         * @param line A linha de comando completa (usada apenas para mensagens de erro).
         * @param it Iterador para a linha de comando. Deve apontar para um caractere de aspas (`"`).
         * @return A string processada.
         * @throws std::runtime_error Caso haja algum erro no processamento da string.
         */
        std::string processStringCase(const std::string& line, std::string::const_iterator& it);


    public:
        Tokenizer(const std::string&);

        std::vector<tokenizer_cli::Token> getTokens();
};



#endif

