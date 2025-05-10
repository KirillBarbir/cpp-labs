#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include "ArgParser.h"

namespace ArgumentParser {

ArgParser::ArgParser(std::string_view name)
    : name_(name) {
}

AddingResult& AddingResult::Default(const char* default_string_value) {
    string_added->default_value = default_string_value;
    return *this;
}

AddingResult& AddingResult::Default(int default_int_value) {
    int_added->default_value_were_assigned = true;
    int_added->default_value = default_int_value;
    return *this;
}

AddingResult& AddingResult::Default(bool default_bool_value) {
    bool_added->default_value = default_bool_value;
    return *this;
}

AddingResult& AddingResult::StoreValue(std::string& string_storage) {
    string_added->string_storage = &string_storage;
    return *this;
}

AddingResult& AddingResult::StoreValue(int& number) {
    int_added->int_storage = &number;
    return *this;
}

AddingResult& AddingResult::StoreValue(bool& flag) {
    bool_added->bool_storage = &flag;
    return *this;
}

AddingResult& AddingResult::StoreValues(std::vector<int>& numbers) {
    int_added->multivalue_int_storage = &numbers;
    int_added->option_info.is_multivalue = true;
    return *this;
}

AddingResult& AddingResult::StoreValues(std::vector<std::string>& strings_storage) {
    string_added->multivalue_string_storage = &strings_storage;
    string_added->option_info.is_multivalue = true;
    return *this;
}

AddingResult& AddingResult::MultiValue(size_t index) {
    if (int_added != nullptr) {
        int_added->option_info.multivalue_minimum_args = index;
        int_added->option_info.is_multivalue = true;
    }
    if (string_added != nullptr) {
        string_added->option_info.multivalue_minimum_args = index;
        string_added->option_info.is_multivalue = true;
    }
    return *this;
}

AddingResult& AddingResult::Positional() {
    if (string_added != nullptr) {
        string_added->option_info.is_positional = true;
        string_added->option_info.positional_order = positionals_added;
        ++positionals_added;
    }

    if (int_added != nullptr) {
        int_added->option_info.is_positional = true;
        int_added->option_info.positional_order = positionals_added;
        ++positionals_added;
    }

    return *this;
}

AddingResult ArgParser::AddStringArgument(char short_opt,
                                          std::string_view long_opt,
                                          std::string_view description) {
    AddingResult out;
    StringOption new_opt;
    new_opt.option_info.long_opt = long_opt;
    new_opt.option_info.short_opt = short_opt;
    new_opt.option_info.description = description;
    new_opt.option_info.type = ArgType::kString;
    string_opts_.push_back(new_opt);
    out.string_added = &(string_opts_.back());
    return out;
}

AddingResult ArgParser::AddStringArgument(std::string_view long_opt, std::string_view description) {
    return AddStringArgument(' ', long_opt, description);
}

AddingResult ArgParser::AddIntArgument(std::string_view long_opt, std::string_view description) {
    return AddIntArgument(' ', long_opt, description);
}

AddingResult ArgParser::AddIntArgument(char short_opt,
                                       std::string_view long_opt,
                                       std::string_view description) {
    AddingResult out;
    IntOption new_opt;
    new_opt.option_info.long_opt = long_opt;
    new_opt.option_info.short_opt = short_opt;
    new_opt.option_info.description = description;
    new_opt.option_info.type = ArgType::kInteger;
    int_opts_.push_back(new_opt);
    out.int_added = &(int_opts_.back());
    return out;
}

AddingResult ArgParser::AddFlag(std::string_view long_opt, std::string_view description) {
    return AddFlag(' ', long_opt, description);
}

AddingResult ArgParser::AddFlag(char short_opt,
                                std::string_view long_opt,
                                std::string_view description) {
    AddingResult out;
    BoolOption new_opt;
    new_opt.option_info.long_opt = long_opt;
    new_opt.option_info.short_opt = short_opt;
    new_opt.option_info.description = description;
    new_opt.option_info.type = ArgType::kFlag;
    bool_opts_.push_back(new_opt);
    out.bool_added = &(bool_opts_.back());
    return out;
}

int ArgParser::Parse(int argc, char** argv) {
    std::vector<std::string> input_arguments;
    for (int i = 0; i < argc; ++i) {
        input_arguments.emplace_back(argv[i]);
    }
    return Parse(input_arguments);
}

void ArgParser::StringValuesUpdate(size_t option_index, const std::string& name_value) {
    if (string_opts_[option_index].option_info.is_multivalue) {
        string_opts_[option_index].multivalue_arguments.push_back(name_value);
    }
    string_opts_[option_index].value = name_value;
    StorageUpdate(string_opts_[option_index], name_value);
}

void ArgParser::IntValuesUpdate(size_t option_index, const std::string& name_value) {
    int_opts_[option_index].were_assigned = true;
    if (int_opts_[option_index].option_info.is_multivalue) {
        int_opts_[option_index].multivalue_arguments.push_back(StringToInt(name_value.c_str()));
    }
    int_opts_[option_index].value = StringToInt(name_value.c_str());
    StorageUpdate(int_opts_[option_index], int_opts_[option_index].value);
}

void ArgParser::FlagValuesUpdate(size_t option_index, const std::string& name_value) {
    if (name_value != "true" && name_value != "false") {
        return;
    }
    bool value = (name_value == "true");
    bool_opts_[option_index].value = value;
    StorageUpdate(bool_opts_[option_index], value);
}

bool ArgParser::ForFlagCases(size_t option_index) {
    if (bool_opts_[option_index].option_info.description == "Display this help and exit") { //help case
        std::cout << HelpDescription();
        return true;
    }
    bool_opts_[option_index].value = true;
    return false;
}

bool ArgParser::UpdateStringsToDefault(StringOption& opt) {
    if (opt.option_info.is_multivalue
        && opt.multivalue_arguments.size() < opt.option_info.multivalue_minimum_args) {
        return false;
    }
    if (!opt.value.empty()) {
        return true;
    }
    if (opt.default_value.empty()) {
        return false;
    }
    if (!opt.option_info.is_multivalue) {
        opt.value = opt.default_value;
        StorageUpdate(opt, opt.default_value);
        return true;
    }
}

bool ArgParser::UpdateIntsToDefault(IntOption& opt) {
    if (opt.option_info.is_multivalue
        && opt.multivalue_arguments.size() < opt.option_info.multivalue_minimum_args) {
        return false;
    }
    if (opt.were_assigned) {
        return true;
    }
    if (!opt.default_value_were_assigned) {
        return false;
    }
    if (!opt.option_info.is_multivalue) {
        opt.value = opt.default_value;
        opt.were_assigned = true;
        StorageUpdate(opt, opt.default_value);
        return true;
    }
}

bool ArgParser::UpdateFlagsToDefault(BoolOption& opt) {
    opt.value = opt.value || opt.default_value;
    StorageUpdate(opt, opt.value);
    return true;
}

bool ArgParser::Parse(const std::vector<std::string>& input_arguments) {
    for (int i = 1; i < input_arguments.size(); ++i) {
        if (input_arguments[i].size() <= 1 || input_arguments[i][0] != '-') {
            size_t ind = FindPositionalOption().first;
            ArgType arg_type = FindPositionalOption().second;
            switch (arg_type) {
                case (ArgType::kString):
                    if (!string_opts_[ind].option_info.is_positional
                        || string_opts_[ind].option_info.positional_order != current_positional) {
                        return false;
                    }
                    StringValuesUpdate(ind, input_arguments[i]);
                    if (!string_opts_[ind].option_info.is_multivalue) {
                        ++current_positional;
                    }
                    break;

                case (ArgType::kInteger):
                    if (!int_opts_[ind].option_info.is_multivalue) {
                        current_positional = int_opts_[ind].option_info.positional_order;
                    }
                    if (!int_opts_[ind].option_info.is_positional
                        || int_opts_[ind].option_info.positional_order != current_positional) {
                        return false;
                    }
                    IntValuesUpdate(ind, input_arguments[i]);
                    if (!int_opts_[ind].option_info.is_multivalue) {
                        ++current_positional;
                    }
                    break;
            }

            continue;
        }

        if (input_arguments[i][1] == '-') { //long option
            size_t j = input_arguments[i].find('=');
            if (j == input_arguments[i].npos) { //flag case
                size_t option_index = FindOption(input_arguments[i].substr(2, input_arguments[i].size() - 2)).first;
                ArgType option_type = FindOption(input_arguments[i].substr(2, input_arguments[i].size() - 2)).second;
                if (option_type != ArgType::kFlag) {
                    ErrorOutput("Flag is not found");
                }
                if (ForFlagCases(option_index)) {
                    return true;
                }
                continue;
            }
            std::string_view option_name = input_arguments[i].substr(2, j - 2);
            std::string name_value = input_arguments[i].substr(j + 1, input_arguments[i].size() - j - 1);
            size_t option_index = FindOption(option_name).first;
            ArgType arg_type = FindOption(option_name).second;
            switch (arg_type) {
                case (ArgType::kString): {
                    StringValuesUpdate(option_index, name_value);
                    break;
                }

                case (ArgType::kInteger): {
                    IntValuesUpdate(option_index, name_value);
                    break;
                }

                case (ArgType::kFlag): {
                    FlagValuesUpdate(option_index, name_value);
                }
            }

        } else { //short option
            if (input_arguments[i].size() > 2 && input_arguments[i][2] == '=') { //-<short_opt>=<value> case
                char option_name = input_arguments[i][1];
                std::string name_value = input_arguments[i].substr(3, input_arguments[i].size() - 3); //discard -<char>=
                size_t option_index = FindOption(option_name).first;
                ArgType arg_type = FindOption(option_name).second;
                switch (arg_type) {
                    case (ArgType::kString): {
                        StringValuesUpdate(option_index, name_value);
                        break;
                    }

                    case (ArgType::kInteger): {
                        IntValuesUpdate(option_index, name_value);
                        break;
                    }

                    case (ArgType::kFlag): {
                        FlagValuesUpdate(option_index, name_value);
                        break;
                    }
                }
            } else if (input_arguments.size() > i + 1 && input_arguments[i + 1][0] != '-'
                && input_arguments[i].size() == 2) {
                //-<short_opt> <value> case
                size_t option_index = FindOption(input_arguments[i][1]).first;
                ArgType arg_type = FindOption(input_arguments[i][1]).second;
                switch (arg_type) {
                    case (ArgType::kString): {
                        StringValuesUpdate(option_index, input_arguments[i + 1]);
                        break;
                    }

                    case (ArgType::kInteger): {
                        IntValuesUpdate(option_index, input_arguments[i + 1]);
                        break;
                    }

                    case (ArgType::kFlag): {
                        FlagValuesUpdate(option_index, input_arguments[i + 1]);
                    }
                }
                ++i;

            } else { //flag case
                for (int j = 1; j < input_arguments[i].size(); ++j) {
                    size_t option_index = FindOption(input_arguments[i][j]).first;
                    ArgType option_type = FindOption(input_arguments[i][j]).second;
                    if (option_type != ArgType::kFlag) {
                        ErrorOutput("Flag is not found");
                    }
                    if (ForFlagCases(option_index)) {
                        return true;
                    }
                    StorageUpdate(bool_opts_[option_index], true);
                }
            }
        }
    }

    for (size_t i = 0; i < string_opts_.size(); ++i) {
        if (!UpdateStringsToDefault(string_opts_[i])) {
            //return false; CHANGE ONLY FOR LABWORK 6
        }
    }

    for (size_t i = 0; i < int_opts_.size(); ++i) {
        if (!UpdateIntsToDefault(int_opts_[i])) {
            return false;
        }
    }

    for (size_t i = 0; i < bool_opts_.size(); ++i) {
        if (!UpdateFlagsToDefault(bool_opts_[i])) {
            return false;
        }
    }
    return true;
}

std::pair<size_t, ArgType> ArgParser::FindOption(std::string_view long_opt) const {
    for (int i = 0; i < string_opts_.size(); ++i) {
        if (string_opts_[i].option_info.long_opt != long_opt) {
            continue;
        }
        return {i, ArgType::kString};
    }

    for (int i = 0; i < int_opts_.size(); ++i) {
        if (int_opts_[i].option_info.long_opt != long_opt) {
            continue;
        }
        return {i, ArgType::kInteger};
    }

    for (int i = 0; i < bool_opts_.size(); ++i) {
        if (bool_opts_[i].option_info.long_opt != long_opt) {
            continue;
        }
        return {i, ArgType::kFlag};
    }
    ErrorOutput("Option is not found");
}

std::pair<size_t, ArgType> ArgParser::FindOption(char short_opt) const {
    for (int i = 0; i < string_opts_.size(); ++i) {
        if (string_opts_[i].option_info.short_opt != short_opt) {
            continue;
        }
        return {i, ArgType::kString};
    }

    for (int i = 0; i < int_opts_.size(); ++i) {
        if (int_opts_[i].option_info.short_opt != short_opt) {
            continue;
        }
        return {i, ArgType::kInteger};
    }

    for (int i = 0; i < bool_opts_.size(); ++i) {
        if (bool_opts_[i].option_info.short_opt != short_opt) {
            continue;
        }
        return {i, ArgType::kFlag};
    }
    ErrorOutput("Short option is not found");
}

std::pair<size_t, ArgType> ArgParser::FindPositionalOption() {
    for (int i = 0; i < string_opts_.size(); ++i) {
        if (!string_opts_[i].option_info.is_positional
            || string_opts_[i].option_info.positional_order != current_positional) {
            continue;
        }
        return {i, ArgType::kString};
    }

    for (int i = 0; i < int_opts_.size(); ++i) {
        if (!int_opts_[i].option_info.is_positional
            || int_opts_[i].option_info.positional_order != current_positional) {
            continue;
        }
        return {i, ArgType::kInteger};
    }
    ErrorOutput("Unable to find positional option");
    return {0, ArgType::kString};
}

std::string ArgParser::GetStringValue(std::string_view long_opt) const {
    return string_opts_[FindOption(long_opt).first].value;
}

std::string ArgParser::GetStringValue(std::string_view long_opt, int ind) const {
    return string_opts_[FindOption(long_opt).first].multivalue_arguments[ind];
}

std::string ArgParser::GetStringValue(char short_opt) const {
    return string_opts_[FindOption(short_opt).first].value;
}

std::string ArgParser::GetStringValue(char short_opt, int ind) const {
    return string_opts_[FindOption(short_opt).first].multivalue_arguments[ind];
}

std::vector<std::string> ArgParser::GetStringMultiValue(char short_opt) const {
    return string_opts_[FindOption(short_opt).first].multivalue_arguments;
}

std::vector<std::string> ArgParser::GetStringMultiValue(std::string_view long_opt) const {
    return string_opts_[FindOption(long_opt).first].multivalue_arguments;
}

void ArgParser::AddOptDescription(std::string& out, const OptionInfo& option_info) const {
    if (option_info.short_opt != ' ') {
        out += "-";
        out.push_back(option_info.short_opt);
    } else {
        out += "  ";
    }
    out += "   ";
    out += "--" + option_info.long_opt + "  ";
    if (option_info.is_multivalue) {
        out +=
            "is multivalue with minimum " + std::to_string(option_info.multivalue_minimum_args)
                + " arguments, ";
    }
    if (option_info.is_positional) {
        out += "is positional, ";
    }
    switch (option_info.type) {
        case (ArgType::kString): {
            out += "is string, ";
            break;
        }

        case (ArgType::kInteger): {
            out += "is int, ";
            break;
        }

        case (ArgType::kFlag): {
            out += "is flag, ";
            break;
        }
    }
    out += option_info.description + "\n";
}

std::string ArgParser::HelpDescription() const {
    std::string out = name_ + "\n";
    out += help_description_;
    for (int i = 0; i < string_opts_.size(); ++i) {
        AddOptDescription(out, string_opts_[i].option_info);
    }
    for (int i = 0; i < int_opts_.size(); ++i) {
        AddOptDescription(out, int_opts_[i].option_info);
    }
    for (int i = 0; i < bool_opts_.size(); ++i) {
        AddOptDescription(out, bool_opts_[i].option_info);
    }
    return out;
}

int ArgParser::GetIntValue(std::string_view long_opt) const {
    return int_opts_[FindOption(long_opt).first].value;
}

int ArgParser::GetIntValue(std::string_view long_opt, int ind) const {
    return int_opts_[FindOption(long_opt).first].multivalue_arguments[ind];
}

int ArgParser::GetIntValue(char short_opt) const {
    return int_opts_[FindOption(short_opt).first].value;
}

int ArgParser::GetIntValue(char short_opt, int ind) const {
    return int_opts_[FindOption(short_opt).first].multivalue_arguments[ind];
}

bool ArgParser::GetFlag(std::string_view long_opt) const {
    return (bool_opts_[FindOption(long_opt).first].value);
}

bool ArgParser::GetFlag(char short_opt) const {
    return (bool_opts_[FindOption(short_opt).first].value);
}

bool ArgParser::Help() const {
    return have_help_;
}

void ArgParser::StorageUpdate(StringOption option, const std::string& value) {
    if (option.option_info.is_multivalue && option.multivalue_string_storage == nullptr) {
        return;
    }

    if (option.option_info.is_multivalue) {
        option.multivalue_string_storage->push_back(value);
    }

    if (option.string_storage == nullptr) {
        return;
    }

    *option.string_storage = value;
}

void ArgParser::StorageUpdate(const IntOption& option, int value) {
    if (option.option_info.is_multivalue && option.multivalue_int_storage == nullptr) {
        return;
    }

    if (option.option_info.is_multivalue) {
        option.multivalue_int_storage->push_back(value);
    }

    if (option.int_storage == nullptr && option.option_info.type == ArgType::kInteger) {
        return;
    }

    *option.int_storage = value;
}

void ArgParser::StorageUpdate(const BoolOption& option, bool value) {
    if (option.bool_storage == nullptr) {
        return;
    }
    *option.bool_storage = value;
}

void ArgParser::ErrorOutput(std::string_view s) const {
    std::cerr << s << std::endl;
    //exit(1);
}

void ArgParser::AddHelp(std::string_view long_opt, const std::string& description) {
    AddHelp(' ', long_opt, description);
}

void ArgParser::AddHelp(char short_opt, std::string_view long_opt, const std::string& description) {
    help_description_ = description + "\n\n";
    have_help_ = true;
    AddFlag(short_opt, long_opt, "Display this help and exit");
}

int ArgParser::StringToInt(const char* value) const {
    char* str_end;
    int out = static_cast<int> (strtol(value, &str_end, 10));
    if (std::strlen(str_end) > 0) {
        ErrorOutput("Invalid argument for int option");
    }
    return out;
}

} // namespace ArgumentParser