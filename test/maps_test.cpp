// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <fplus/fplus.hpp>
#include <vector>

namespace {

    auto is_even_int = [](int x){ return x % 2 == 0; };
    typedef std::vector<int> IntVector;
}

TEST_CASE("maps_test, transform_map_values")
{
    using namespace fplus;
    typedef std::vector<std::pair<std::string, int>> StringIntPairs;
    StringIntPairs stringIntPairs = {{"a", 1}, {"a", 2}, {"b", 6}, {"a", 4}};
    auto stringIntPairsAsMapGrouped = pairs_to_map_grouped(stringIntPairs);
    auto groupNameToMedianMap = transform_map_values(median<std::vector<int>>, stringIntPairsAsMapGrouped);
    typedef std::map<std::string, int> StringIntMap;
    REQUIRE_EQ(groupNameToMedianMap, StringIntMap({{"a", 2}, {"b", 6}}));
}

TEST_CASE("maps_test, map functions")
{
    using namespace fplus;
    typedef std::map<int, std::string> IntStringMap;
    typedef std::map<std::string, int> StringIntMap;
    IntStringMap intStringMap = {{1, "2"}, {4, "53"}, {7, "21"}};
    StringIntMap stringIntMap = {{ "2", 1}, { "53", 4}, { "21", 7}};
    REQUIRE_EQ(swap_keys_and_values(intStringMap), stringIntMap);

    typedef std::vector<std::string> StringVector;
    REQUIRE_EQ(get_map_keys(intStringMap), IntVector({1, 4, 7}));
    REQUIRE_EQ(get_map_values(intStringMap), StringVector({"2", "53", "21"}));

    typedef std::unordered_map<int, std::string> IntStringUnorderedMap;
    typedef std::unordered_map<std::string, int> StringIntUnorderedMap;
    IntStringUnorderedMap intStringUnorderedMap = { { 1, "2" },{ 4, "53" },{ 7, "21" } };
    StringIntUnorderedMap stringIntUnorderedMapSwapped = { { "2", 1 },{ "53", 4 },{ "21", 7 } };
    REQUIRE_EQ(swap_keys_and_values(intStringUnorderedMap), stringIntUnorderedMapSwapped);
    REQUIRE_EQ(convert_container<IntStringUnorderedMap>(intStringMap), intStringUnorderedMap);
    REQUIRE_EQ(convert_container<IntStringMap>(intStringUnorderedMap), intStringMap);

    std::vector<int> mapInts = { 1, 4, 7 };
    std::vector<std::string> mapStrings = { "2", "53", "21" };
    REQUIRE_EQ(create_map(mapInts, mapStrings), intStringMap);
    REQUIRE_EQ(create_unordered_map(mapInts, mapStrings), intStringUnorderedMap);

    IntStringMap intsAsStringsMap = {{1, "1"}, {4, "4"}, {7, "7"}};
    REQUIRE_EQ(create_map_with(show<int>, mapInts), intsAsStringsMap);
    IntStringUnorderedMap intsAsStringsUnorderedMap = {{1, "1"}, {4, "4"}, {7, "7"}};
    REQUIRE_EQ(create_unordered_map_with(show<int>, mapInts), intsAsStringsUnorderedMap);

    const auto is_int_string_map_key_even =
        [&](const IntStringMap::value_type& p) -> bool
    {
        return is_even_int(p.first);
    };
    REQUIRE_EQ(keep_if(is_int_string_map_key_even, IntStringMap({{4, "4"}, {7, "7"}})), IntStringMap({{4, "4"}}));

    REQUIRE_EQ(get_from_map(intStringMap, 1), just<std::string>("2"));
    REQUIRE_EQ(get_from_map(intStringMap, 9), nothing<std::string>());
    REQUIRE_EQ(get_from_map_with_def(intStringMap, std::string("n/a"), 1), "2");
    REQUIRE_EQ(get_from_map_with_def(intStringMap, std::string("n/a"), 9), "n/a");
    REQUIRE_EQ(map_contains(intStringMap, 1), true);
    REQUIRE_EQ(map_contains(intStringMap, 9), false);

    IntStringMap union_map_1 = {{0, "a"}, {1, "b"}};
    IntStringMap union_map_2 = {{0, "c"}, {2, "d"}};
    IntStringMap union_map_res = {{0, "a"}, {1, "b"}, {2, "d"}};
    IntStringMap union_map_with_res = {{0, "ac"}, {1, "b"}, {2, "d"}};
    REQUIRE_EQ(map_union(union_map_1, union_map_2), union_map_res);
    REQUIRE_EQ(map_union_with(append<std::string>, union_map_1, union_map_2), union_map_with_res);

    typedef std::map<std::string::value_type, int> CharIntMap;
    CharIntMap charIntMap = {{'a', 1}, {'b', 2}, {'A', 3}, {'C', 4}};
    const auto is_upper = [](std::string::value_type c) -> bool
    {
        return std::isupper(c);
    };
    const auto is_lower = [](std::string::value_type c) -> bool
    {
        return std::islower(c);
    };
    REQUIRE_EQ(map_keep_if(is_upper, charIntMap), CharIntMap({{'A', 3}, {'C', 4}}));
    REQUIRE_EQ(map_drop_if(is_lower, charIntMap), CharIntMap({{'A', 3}, {'C', 4}}));
    typedef std::vector<std::string::value_type> CharVector;
    REQUIRE_EQ(map_keep(CharVector({'b', 'F'}), charIntMap), CharIntMap({{'b', 2}}));
    REQUIRE_EQ(map_drop(CharVector({'a', 'A', 'C', 'F'}), charIntMap), CharIntMap({{'b', 2}}));

    typedef std::vector<CharIntMap> CharIntMaps;
    typedef std::vector<maybe<int>> MaybeInts;
    REQUIRE_EQ(
        map_pluck('a', CharIntMaps({{{'a',1}, {'b',2}}, {{'a',3}}, {{'c',4}}})),
        MaybeInts({1, 3, {}}));
}