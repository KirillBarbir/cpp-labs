#pragma once

#include <string>
#include <vector>


namespace ArgumentParser {

enum class ArgType {
    kInteger,
    kString,
    kFlag
};

struct OptionInfo {
    //names
    std::string long_opt;
    char short_opt = 0;

    //values
    std::string description;
    ArgType type;

    //multivalue information
    size_t multivalue_minimum_args = 0;
    bool is_multivalue = false;

    //positional information
    bool is_positional = false;
    size_t positional_order;
};

struct StringOption{
  OptionInfo option_info;

  //values
  std::string default_value;
  std::string value;
  std::vector<std::string> multivalue_arguments;

  //storages
  std::string* string_storage = nullptr;
  std::vector<std::string>* multivalue_string_storage = nullptr;
};

struct IntOption{
  OptionInfo option_info;

  //values
  int default_value;
  int value;
  std::vector<int> multivalue_arguments;

  bool were_assigned = false;
  bool default_value_were_assigned = false;

  //storages
  int* int_storage = nullptr;
  std::vector<int>* multivalue_int_storage = nullptr;
};

struct BoolOption{
  OptionInfo option_info;

  //values
  bool default_value = false;
  bool value = false;

  bool* bool_storage = nullptr;
};

class AddingResult {
    friend class ArgParser;
  public:
    AddingResult& Default(const char* default_string_value);
    AddingResult& Default(int default_int_value);
    AddingResult& Default(bool default_bool_value);

    AddingResult& StoreValue(std::string& string_storage);
    AddingResult& StoreValue(int& number);
    AddingResult& StoreValue(bool& flag);
    AddingResult& StoreValues(std::vector<int>& numbers);
    AddingResult& StoreValues(std::vector<std::string>& strings_storage);

    AddingResult& MultiValue(size_t index = 0);
    AddingResult& Positional();

  private:
    size_t positionals_added = 0;
    StringOption* string_added = nullptr;
    IntOption* int_added = nullptr;
    BoolOption* bool_added = nullptr;
};

class ArgParser {
    friend class AddingResult;
  public:
    explicit ArgParser(std::string_view name);

    AddingResult AddStringArgument(std::string_view long_opt, std::string_view description = "");
    AddingResult AddStringArgument(char short_opt,
                                   std::string_view long_opt = "",
                                   std::string_view description = "");
    AddingResult AddIntArgument(std::string_view long_opt, std::string_view description = "");
    AddingResult AddIntArgument(char short_opt,
                                std::string_view long_opt = "",
                                std::string_view description = "");

    AddingResult AddFlag(std::string_view long_opt, std::string_view description = "");
    AddingResult AddFlag(char short_opt,
                         std::string_view long_opt = "",
                         std::string_view description = "");

    std::string GetStringValue(std::string_view long_opt) const;
    std::string GetStringValue(std::string_view long_opt, int ind) const;
    std::string GetStringValue(char short_opt) const;
    std::string GetStringValue(char short_opt, int ind) const;

    int GetIntValue(std::string_view long_opt) const;
    int GetIntValue(std::string_view long_opt, int ind) const;
    int GetIntValue(char short_opt) const;
    int GetIntValue(char short_opt, int ind) const;

    bool GetFlag(std::string_view long_opt) const;
    bool GetFlag(char short_opt) const;

    bool UpdateStringsToDefault(StringOption& opt);
    bool UpdateIntsToDefault(IntOption& opt);
    bool UpdateFlagsToDefault(BoolOption& opt);

    int Parse(int argc, char**);
    bool Parse(const std::vector<std::string>& input_arguments);

    bool Help() const;
    std::string HelpDescription() const;

    void AddHelp(std::string_view long_opt, const std::string& description = " ");
    void AddHelp(char short_opt,
                 std::string_view long_opt,
                 const std::string& description = " ");

  private:
    std::string name_;
    std::string help_description_ = "There is no help added yet";

    std::vector<StringOption> string_opts_;
    std::vector<IntOption> int_opts_;
    std::vector<BoolOption> bool_opts_;

    bool have_help_ = false;

    size_t current_positional = 0;

    std::pair<size_t, ArgType> FindOption(char short_opt) const;
    std::pair<size_t, ArgType> FindOption(std::string_view x) const;
    std::pair<size_t, ArgType> FindPositionalOption();

    bool ForFlagCases(size_t option_index);
    void ErrorOutput(std::string_view s) const;

    void StringValuesUpdate(size_t option_index, const std::string& name_value);
    void IntValuesUpdate(size_t option_index, const std::string& name_value);
    void FlagValuesUpdate(size_t option_index, const std::string& name_value);

    void StorageUpdate(StringOption option, const std::string& value);
    void StorageUpdate(const IntOption& option, int value);
    void StorageUpdate(const BoolOption&, bool value);

    int StringToInt(const char* value) const;

    void AddOptDescription(std::string& out, const OptionInfo &option_info) const;
};

} // namespace ArgumentParser