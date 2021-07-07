#include "fsmpp2/meta.hpp"

/* Here are only compil-time tests verified by static_assert */

using namespace fsmpp2::meta;

using list_0 = type_list<char, int, float>;

// type_list_size
static_assert(type_list_size(list_0{}) == 3, "three element list");

// type_list_has
static_assert(type_list_has<char>(list_0{}), "has char");
static_assert(type_list_has<int>(list_0{}), "has int");
static_assert(type_list_has<float>(list_0{}), "has float");
static_assert(type_list_has<double>(list_0{}) == false, "hasn't double");
static_assert(type_list_has<unsigned char>(list_0{}) == false, "hasn't unsiogned char");

// type_list_index
static_assert(type_list_index<char>(list_0{}) == 0, "char at index 0");
static_assert(type_list_index<int>(list_0{}) == 1, "char at index 1");
static_assert(type_list_index<float>(list_0{}) == 2, "char at index 2");
static_assert(type_list_index<double>(list_0{}) == 3, "not present at index == sizeof");
static_assert(type_list_index<unsigned char>(list_0{}) == 3, "not present at index == sizeof");

// type_list_type
static_assert(std::is_same_v<type_list_type<0, list_0>::type, char>, "char at index 0");
static_assert(std::is_same_v<type_list_type<1, list_0>::type, int>, "int at index 1");
static_assert(std::is_same_v<type_list_type<2, list_0>::type, float>, "float at index 2");
// TODO: static_assert(std::is_same_v<type_list_type<3, list_0>::type, double> == ???, "??");

// type_list_first
static_assert(std::is_same_v<typename type_list_first<list_0>::type, char>, "first type should be char");