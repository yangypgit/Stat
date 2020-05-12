#include <string>
#include <queue>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using TableMap = std::map<std::string, json>;

struct node
{
    std::string table_name;
    size_t type;
    json field;
};

class HcsStat
{
public:
    HcsStat() {}
    ~HcsStat() {}

    enum FIELD_TYPE{INVARIANT = 0, COMMONLY, ASSOCIATED};
    size_t set_invariant_field(const std::string& table_name, const std::string& key, const std::string& value);
    size_t set_invariant_field(const std::string& table_name, const json& js);
    size_t set_commonly_field(const std::string& table_name, const std::string& key, const uint64_t& value);
    size_t set_commonly_field(const std::string& table_name, const json& js);
    size_t set_associated_field(const std::string& table_name, const std::string& key, const uint64_t& value);
    size_t set_associated_field(const std::string& table_name, const json& js);

    void work();
    void insert_mysql();
private:
    void stat(node& n);

    std::queue<node> queue_;
    TableMap table_list_;
};
