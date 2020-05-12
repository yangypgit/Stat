#include <iostream>

#include "hcsstat.hpp"

int main()
{
    HcsStat s;
    s.set_invariant_field("table_name", "key1", "value1");
    s.set_commonly_field("table_name", "xdr_num", 5);
    s.set_commonly_field("table_name", "xdr_num", 5);

    {
        json js;
        js["key2"] = "value2";
        js["key3"] = "value3";
        s.set_invariant_field("table_name", js);
    }

    {
        json js;
        js["xdr_in"] = 5;
        js["xdr_num"] = 2;
        s.set_commonly_field("table_name", js);
    }

    s.work();
    s.insert_mysql();

    return 0;
}
