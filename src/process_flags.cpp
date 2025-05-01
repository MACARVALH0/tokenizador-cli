#include <tokenizer-cli/tokenizer-cli.hpp>
#include <tokenizer-cli/flag-struct.hpp>



/**
 * @brief Retorna um elemento (bool/string) para ocupar o espaço de "argumento" da flag.
 * 
 * @tparam Flag_Config_Name Enumerador da lista de nomes de configurações de flags.
 * @tparam Flag_Config_Value Enumerador da lista de valores de configurações de flags.
 * 
 * @param   rule    Elemento `FlagRule` cuja regra bateu com a busca.
 * @param   it      Iterador apontando para o elemento, da lista de tokens, que combina com a regra encontrada.
 * @param   end     Iterador que aponta para o final da lista de tokens.
 * @return          tokenizer_cli::flag_arg; `bool` ou `std::string` correspondente ao atributo de "argumento" da flag.
 */
template <typename Flag_Config_Name, typename Flag_Config_Value>
tokenizer_cli::flag_arg getFlagArgument
(
    const tokenizer_cli::FlagRule<Flag_Config_Name, Flag_Config_Value>& rule,
    std::vector<tokenizer_cli::Token>::iterator it,
    std::vector<tokenizer_cli::Token>::iterator end
)
{
    // Iterador apontando para o próximo item.
    auto next_it = it+1 != end ? it+1 : it;

    /*
        Se `rule.needs_argument == std::nullopt`, então o argumento é opcional.
        Logo, segue-se a análise de preenchimento opcional, que retorna:
        a. O token seguinte, caso o próximo item seja do tipo STRING, ou
        b. Uma string vazia, caso a condição anterior não seja atendida.
    */
    if(rule.needs_argument == std::nullopt){ return (next_it != end && next_it->type == OpTokenType::STRING) ? next_it->content : ""; }

    // Caso a regra não exija argumento, retorna uma string vazia.
    if(!rule.needs_argument){ return std::string(""); }

    // FIXME Seria mais adequado que a função retornasse um erro caso a flag necessitasse de um argumento e não o encontrasse.
    if(rule.needs_argument)
    {
        if(next_it != end && next_it->type == OpTokenType::STRING)
        { return next_it->content; } // Retorna std::string

        else true; // Retorna booleano
    }

    return "";
};





template <typename Flag_Config_Name, typename Flag_Config_Value>
tokenizer_cli::flag_config_set<Flag_Config_Name, Flag_Config_Value>
getFlagSetup(std::vector<tokenizer_cli::Token>& tokens)
{
    // Caso não hajam outros tokens na linha de comando.
    if(tokens.empty()){ return flag_setup_map{}; }

    // Define o objeto de set de flags.
    FlagSet<Flag_Config_Name, Flag<Flag_Config_Value> set;

    // FIXME Exemplo. Isso deve ser passado como PARÂMETRO.
    // Definição da minha estrutura de regras.
    std::vector<FlagRule<Flag_Config_Name, Flag_Config_Value>> rules =
    {
        { { "-s", "--short" },  Flag_Config_Value::SHORT,    Flag_Config_Name::SIZE,    true },
        { { "-l", "--long"  },  Flag_Config_Value::LONG,     Flag_Config_Name::SIZE }
    };


    auto token_it = tokens.begin(); // Iterador que percorrerá o vetor.
    const auto end = tokens.end();  // Cache do iterador marcando o final do vetor.

    // Loop sobre todods os elementos da lista de tokens.
    while(token_it != end)
    {
        if(token_it->type != OpTokenType::FLAG){ token_it++; continue; }

        else
        {
            for(const FlagRule<Flag_Config_Name, Flag_Config_Value>& rule : rules)
            {
                // Caso o conteúdo do token esteja incluso em algum grupo de `rules`.
                if(rule.group.find(token_it->content) != rule.group.end())
                {
                    // Armazena o atributo `arg` da flag. 
                    tokenizer_cli::flag_arg flag_argument = getFlagArgument(rule, token_it, end);

                    // Declara a flag baseado na regra testada.
                    const Flag<Flag_Config_Value> flag(rule.value, flag_argument);

                    // Adiciona a flag atual ao conjunto de FlagSet.
                    set.setFlag(flag, rule.config_name);

                    tokens.erase(token_it);
                }

                else continue;
            }
        }
    }

    // Retorna os valores armazenados no set de flags.
    return set.retrieve();
}


// static std::string toString_Configuration(Configuration config)
// {
//     switch(config)
//     {
//         case Configuration::SIZE: return "SIZE"; break;
//         default: return "UNKNOWN";
//     }
// }