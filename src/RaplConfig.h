#include <map>
#include <string>

class RaplConfig {
    private:
        std::map<std::string, std::string> config;
        std::string config_name = "config";
    
    public:
        RaplConfig() = default;

        void parseConfig();
        uint64_t get_msr(std::string);
        bool get_bool(std::string);
        int get_number(std::string);
        std::string get_string(std::string);
};