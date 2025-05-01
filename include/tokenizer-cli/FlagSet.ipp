#pragma once

namespace tokenizer_cli
{
    template <typename Flag_Config_Name, typename Flag_Config_Value>
    void FlagSet<Flag_Config_Name, Flag_Config_Value>::setFlag
    (
        const Flag<Flag_Config_Value>& flag,
        Flag_Config_Name name
    )
    {
        if(map.find(config_name) != map.end()) throw std::runtime_error("<# Múltiplas flags de definição do modo de escrita/reescrita da entrada.\n");
        else map[config_name] = flag;
    }

    template <typename Flag_Config_Name, typename Flag_Config_Value>
    flag_config_set<Flag_Config_Name>
    FlagSet<Flag_Config_Name, Flag_Config_Value>::retrieve()
    {
        return map;
    }
}