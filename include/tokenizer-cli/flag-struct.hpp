#ifndef FLAG_PROCESS
#define FLAG_PROCESS

#include <variant>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <optional>


namespace tokenizer_cli
{
    /**
     * @brief Mapa de configuração de flags ativas.
     * 
     * @tparam Flag_Config_Name Enumerador da lista de nomes de configurações de flags.
     * @tparam Flag_Config_Value Enumerador da lista de valores de configurações de flags.
     */
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    using flag_config = std::enable_if_t<
        std::is_enum_v<Flag_Config_Name> && std::is_enum_v<Flag_Config_Value>,
        std::pair<Flag_Config_Name, Flag_Config_Value>
    >;

    using flag_arg = std::variant<std::string, bool>; // Um variant dos tipos `string` e `bool`.


    /**
     * @brief Estrutura de dados de uma Flag.
     * Armazena um `value` e um `arg`,
     * que representam, respectivamente, a qualidade de alguma configuração e seu argumento.
     * 
     * @tparam Flag_Config_Value Enumerador da lista de nomes de configurações de flags.
     * 
     * @example `Flag flag("SHORT", "Conteúdo a ser adicionado sob o viés 'SHORT'.")`
     */
    template <typename Flag_Config_Value>
    struct Flag
    {
        static_assert(std::is_enum_v<Flag_Config_Value>, "Flag: O tipo deve ser um enum.");

        Flag_Config_Value value;
        flag_arg arg;

        Flag(Flag_Config_Value value, flag_arg arg) : value(value), arg(arg) {}
    };


    /**
     * @brief O conjunto de `flag_config` individuais.
     * 
     * @tparam Flag_Config_Name Enumerador da lista de nomes de configurações de flags.
     * @tparam Flag_Config_Value Enumerador da lista de valores de configurações de flags.
     */
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    using flag_config_set = std::enable_if_t<
        std::is_enum_v<Flag_Config_Name> && std::is_enum_v<Flag_Config_Value>,
        std::unordered_map<Flag_Config_Name, Flag<Flag_Config_Value>>
    >;



    /**
     * @brief Abstração para uma regra de captura de tipo flag.
     * 
     * @tparam Flag_Config_Name Enumerador da lista de nomes de configurações de flags.
     * @tparam Flag_Config_Value Enumerador da lista de valores de configurações de flags.
     * 
     * @param group Um lista de nomes que identificam a tag entre os tokens.
     * @param config_name Elemento de um enumerador que identifica o nome de uma determinada configuração de flag.
     * @param value Elemento de um enumerador que identifica o valor atribuído a determinada configuração de flag.
     * @param needs_argument Opcional. Indica se a flag leva um argumento obrigatório.
     * @example { { "-s", "--short" },  FlagValue::SHORT,    Configuration::SIZE,    true }
     */
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    struct FlagRule
    {
        static_assert(std::is_enum_v<Flag_Config_Name>, "Flag: O tipo `Flag_Name_Enum` deve ser um enum.");
        static_assert(std::is_enum_v<Flag_Config_Value>, "Flag: O tipo `Flag_Value_Enum` deve ser um enum.");

        std::unordered_set<std::string> group;
        Flag_Config_Name config_name;
        Flag_Config_Value value;
        std::optional<bool> needs_argument;

        // Construtor de FlagRule
        FlagRule
        (
            std::unordered_set<std::string> flag_name_list,
            Flag_Config_Name flag_config_name,
            Flag_Config_Value flag_config_value,
            bool needs_arg = false
        )
        : group(flag_name_list), config_name(flag_config_name), value(flag_config_value), needs_argument(needs_arg) {}
    };



    /**
     * @brief Uma lista de `FlagRule`s atribuídas através de flags.
     * 
     * @tparam Flag_Config_Name Enumerador da lista de nomes de configurações de flags.
     * @tparam Flag_Config_Value Enumerador da lista de valores de configurações de flags.
     */
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    using FlagConfigurationList = std::enable_if_t<
        std::is_enum_v<Flag_Config_Name> && std::is_enum_v<Flag_Config_Value>,
        std::vector<FlagRule<Flag_Config_Name, Flag_Config_Value>>
    >;



    /**
     * @brief 
     * 
     * @tparam Flag_Config_Name Enumerador da lista de nomes de configurações de flags.
     * @tparam Flag_Config_Value Enumerador da lista de valores de configurações de flags.
     */
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    class FlagSet
    {
        static_assert(std::is_enum_v<Flag_Config_Name>, "Flag_Config_Name deve ser um enumerador.");
        static_assert(std::is_enum_v<Flag_Config_Value>, "Flag_Config_Name deve ser um enumerador.");

        private:

            flag_config_set<Flag_Config_Name> map{};

        public:

            void setFlag(const Flag<Flag_Config_Value>& flag, Flag_Config_Name config_name);
            flag_config_set<Flag_Config_Name, Flag<Flag_Config_Value>> retrieve();
    };



    /**
     * @brief Um par de dados {`Flag_Config_Name: Flag`} onde 
     * `Flag_Config_Name` representa o nome da configuração e
     * `Flag` tem a flag que modifica o valor dessa configuração.
     * 
     * @tparam Flag_Config_Name Enumerador dos nomes de configuração possíveis.
     * @tparam Flag_Config_Value Enumerador do conjunto de valores de configuração válidos.
     */
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    struct FContextConfiguration
    {
        static_assert(std::is_enum_v<Flag_Config_Name>, "Flag_Config_Name deve ser um enumerador.");
        static_assert(std::is_enum_v<Flag_Config_Value>, "Flag_Config_Name deve ser um enumerador.");

        Flag_Config_Name configuration;
        bool is_obligatory;
        bool exists;
        Flag flag;

        FContextConfiguration(Flag_Config_Name config, bool obligatory)
        : configuration(config), obligatory(obligatory) {}
    };

}



#include <tokenizer-cli/FlagSet.ipp>

#endif