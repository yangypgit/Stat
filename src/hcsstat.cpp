#include "hcsstat.hpp"
#include <iostream>

/*
 *      {
 *          table1 : {
 *              key1 : value1,
 *              key2 : value2,
 *              key3 : value3
 *          },
 *          table2 : {
 *              key1 : value1,
 *              key2 : value2,
 *              key3 : value3,
 *              interface : [1, 2, 3],
 *              xdr_in : [1, 2, 3],
 *              xdr_out : [1, 2, 3]
 *          }
 *      }
 *
 */

std::string get_string_value(const json& js, const std::string &key)
{
    std::string v;

    if (js.find(key.data()) != js.end())
    {
        if (js.at(key.data()).is_string())
        {
            v = js[key.data()].get<std::string>();
        }
        else if (js.at(key.data()).is_number())
        {
            if (js.at(key.data()).is_number_integer())
            {
                int x = js[key.data()].get<int>();
                v = std::to_string(x);
            }
            else if (js.at(key.data()).is_number_float())
            {
                float x = js[key.data()].get<float>();
                v = std::to_string(x);
            }
        }
    }
    else
    {
        std::cout << "no key " << key << "\n";
    }

    return v;
}

template <typename T>
node get_json(const std::string& table_name, const std::string& key, T& value, int type)
{
    node n;
    n.table_name = table_name;
    n.type = type;
    n.field[key] = value;

    return n;
}

node get_json(const std::string& table_name, const json& js, int type)
{
    node n;
    n.table_name = table_name;
    n.type = type;
    n.field = js;

    return n;
}

size_t HcsStat::set_invariant_field(const std::string& table_name, const std::string& key, const std::string& value)
{
    node n = get_json(table_name, key, value, INVARIANT);
    queue_.push(n);

    return 0;
}

size_t HcsStat::set_invariant_field(const std::string& table_name, const json& js)
{
    node n = get_json(table_name, js, INVARIANT);
    queue_.push(n);

    return 0;
}

size_t HcsStat::set_commonly_field(const std::string& table_name, const std::string& key, const uint64_t& value)
{
    node n = get_json(table_name, key, value, COMMONLY);
    queue_.push(n);

    return 0;
}

size_t HcsStat::set_commonly_field(const std::string& table_name, const json& js)
{
    node n = get_json(table_name, js, COMMONLY);
    queue_.push(n);

    return 0;
}

size_t HcsStat::set_associated_field(const std::string& table_name, const std::string& key, const uint64_t& value)
{
    node n = get_json(table_name, key, value, ASSOCIATED);
    queue_.push(n);

    return 0;
}

size_t HcsStat::set_associated_field(const std::string& table_name, const json& js)
{
    node n = get_json(table_name, js, ASSOCIATED);
    queue_.push(n);

    return 0;
}

void HcsStat::work()
{
    while (queue_.size() > 0)
    {
        // 统计
        node n = queue_.front();
        stat(n);
        queue_.pop();

        // 插入数据库
        // insert_mysql();
    }
}

void HcsStat::stat(node& n)
{
    TableMap::iterator it = table_list_.find(n.table_name);
    if (it == table_list_.end())
    {
        // add table
        std::pair<TableMap::iterator, bool> pair = table_list_.insert(std::make_pair(n.table_name, n.field));
        if (!pair.second)
        {
            // insert err
            std::cout << "insert error!!!" << '\n';
            return ;
        }
        std::cout << n.field.dump() << '\n';
    }
    else
    {
        // update
        switch (n.type)
        {
        case INVARIANT:
            {
                for (json::iterator jit = n.field.begin(); jit != n.field.end(); ++jit)
                {
                    it->second[jit.key()] = jit.value();
                    std::cout << it->second.dump() << '\n';
                }
            }
            break;
        case COMMONLY:
            {
                for (json::iterator jit = n.field.begin(); jit != n.field.end(); ++jit)
                {
                    // 如果value不是整数型暂时先直接丢弃
                    if (!jit.value().is_number())
                    {
                        continue;
                    }

                    // 遍历json 找到对应的字段 如果有就更新，没有就添加
                    uint64_t field = 0;
                    if (it->second.find(jit.key()) != it->second.end())
                    {
                        field = it->second.at(jit.key()).get<uint64_t>();
                    }

                    it->second[jit.key()] = field + jit.value().get<uint64_t>();
                    std::cout << it->second.dump() << '\n';
                }
            }
            break;
        case ASSOCIATED:
            {
            }
            break;
        default:
            break;
        }
    }
}

void HcsStat::insert_mysql()
{
    std::string sql("INSERT INTO ");
    TableMap::iterator it = table_list_.begin();
    for (; it != table_list_.end(); ++it)
    {
        sql.resize(12);
        sql.append(it->first);

        std::string key(" ("), value(" VALUE(");
        for (json::iterator jit = it->second.begin(); jit != it->second.end(); ++jit)
        {
            key.append(jit.key());
            key.append(", ");

            value.append(get_string_value(it->second, jit.key()));
            value.append(", ");
        }

        key.resize(key.length() - 2);
        key.append(")");
        value.resize(value.length() - 2);
        value.append(")");

        sql.append(key);
        sql.append(value);
        std::cout << sql << '\n';
    }
}

