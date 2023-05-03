// Andrew Naplavkov

// clang-cl workaround
#ifndef __cpp_consteval
#define __cpp_consteval 201811L
#endif

#include <cctype>
#include <iostream>
#include <random>
#include <source_location>
#include <sstream>
#include <step20/detail/read_file.hpp>
#include <step20/edit_distance.hpp>
#include <step20/example/diff/diff.hpp>
#include <step20/example/suffix_tree_viz/suffix_tree_viz.hpp>
#include <step20/longest_common_subsequence.hpp>
#include <step20/longest_common_substring.hpp>
#include <step20/longest_repeated_substring.hpp>
#include <step20/substring_search.hpp>
#include <step20/suffix_array.hpp>
#include <step20/suffix_tree.hpp>

using namespace step20;
using namespace std::literals;

void log(std::string_view msg,
         std::source_location loc = std::source_location::current())
{
    std::cerr << loc.file_name() << "(" << loc.line() << ") `"
              << loc.function_name() << "`: " << msg << std::endl;
}

void check(bool cond,
           std::source_location loc = std::source_location::current())
{
    if (cond)
        return;
    log("fail", loc);
    std::terminate();
}

std::string generate_random_string(size_t len)
{
    static auto gen = std::mt19937{std::random_device{}()};
    static auto dist = std::uniform_int_distribution<int>{'a', 'z'};
    auto result = std::string{};
    result.reserve(len);
    std::generate_n(std::back_inserter(result), len, [] { return dist(gen); });
    return result;
}

using maybe_char = std::optional<char>;
using maybe_char_pairs = std::vector<std::pair<maybe_char, maybe_char>>;

namespace case_insensitive {

struct equal_to {
    bool operator()(unsigned char lhs, unsigned char rhs) const
    {
        return std::tolower(lhs) == std::tolower(rhs);
    }
};

struct less {
    bool operator()(unsigned char lhs, unsigned char rhs) const
    {
        return std::tolower(lhs) < std::tolower(rhs);
    }
};

}  // namespace case_insensitive

void test_edit_distance_case_insensitive()
{
    log("run");
    struct {
        std::string_view lhs;
        std::string_view rhs;
        maybe_char_pairs expect;
    } tests[] = {
        {"SUNDAY",
         "saturday",
         maybe_char_pairs{
             {'S', 's'},
             {std::nullopt, 'a'},
             {std::nullopt, 't'},
             {'U', 'u'},
             {'N', 'r'},
             {'D', 'd'},
             {'A', 'a'},
             {'Y', 'y'},
         }},
        {"GCGTATGAGGCTAACGC",
         "GCTATGCGGCTATACGC",
         maybe_char_pairs{
             {'G', 'G'},
             {'C', 'C'},
             {'G', std::nullopt},
             {'T', 'T'},
             {'A', 'A'},
             {'T', 'T'},
             {'G', 'G'},
             {'A', 'C'},
             {'G', 'G'},
             {'G', 'G'},
             {'C', 'C'},
             {'T', 'T'},
             {'A', 'A'},
             {std::nullopt, 'T'},
             {'A', 'A'},
             {'C', 'C'},
             {'G', 'G'},
             {'C', 'C'},
         }},
        {"Hyundai",
         "Honda",
         maybe_char_pairs{
             {'H', 'H'},
             {'y', std::nullopt},
             {'u', 'o'},
             {'n', 'n'},
             {'d', 'd'},
             {'a', 'a'},
             {'i', std::nullopt},
         }},
    };
    for (auto& [lhs, rhs, expect] : tests) {
        auto result = maybe_char_pairs{};
        edit_distance::zip(
            lhs, rhs, std::back_inserter(result), case_insensitive::equal_to{});
        check(result == expect);
    }
}

void test_edit_distance_hello_world()
{
    log("run");
    auto result = maybe_char_pairs{};
    edit_distance::zip("this"sv, "has"sv, std::back_inserter(result));
    check(result == maybe_char_pairs{
                        {'t', std::nullopt},
                        {'h', 'h'},
                        {'i', 'a'},
                        {'s', 's'},
                    });
}

void test_example_diff()
{
    log("run");
    auto expect = R"(@@ -1,0 +1,6 @@
+This is an important
+notice! It should
+therefore be located at
+the beginning of this
+document!
+
@@ -11,5 +17,0 @@
-This paragraph contains
-text that is outdated.
-It will be deleted in the
-near future.
-
@@ -17,1 +18,1 @@
-check this dokument. On
+check this document. On
@@ -25,0 +26,4 @@
+
+This paragraph contains
+important new additions
+to this document.
)"sv;
    auto result = std::ostringstream{};
    diff::dump(read_file("../example/diff/file1.txt"),
               read_file("../example/diff/file2.txt"),
               result);
    check(result.str() == expect);
}

void test_example_suffix_tree_viz()
{
    log("run");
    struct {
        std::string_view str;
        std::string_view expect;
    } tests[] = {
        {"abcabxabcd$", R"(digraph "abcabxabcd$" {
rankdir=LR
_0 [shape=point]
10 [shape=plaintext]
_0->10 [label="$"]
_1 [shape=point]
_0->_1 [label="ab"]
_3 [shape=point]
_1->_3 [label="c"]
0 [shape=plaintext]
_3->0 [label="abxabcd$"]
6 [shape=plaintext]
_3->6 [label="d$"]
3 [shape=plaintext]
_1->3 [label="xabcd$"]
_2 [shape=point]
_0->_2 [label="b"]
_4 [shape=point]
_2->_4 [label="c"]
1 [shape=plaintext]
_4->1 [label="abxabcd$"]
7 [shape=plaintext]
_4->7 [label="d$"]
4 [shape=plaintext]
_2->4 [label="xabcd$"]
_5 [shape=point]
_0->_5 [label="c"]
2 [shape=plaintext]
_5->2 [label="abxabcd$"]
8 [shape=plaintext]
_5->8 [label="d$"]
9 [shape=plaintext]
_0->9 [label="d$"]
5 [shape=plaintext]
_0->5 [label="xabcd$"]
}
)"},
        {"banana$", R"(digraph "banana$" {
rankdir=LR
_0 [shape=point]
6 [shape=plaintext]
_0->6 [label="$"]
_3 [shape=point]
_0->_3 [label="a"]
5 [shape=plaintext]
_3->5 [label="$"]
_1 [shape=point]
_3->_1 [label="na"]
3 [shape=plaintext]
_1->3 [label="$"]
1 [shape=plaintext]
_1->1 [label="na$"]
0 [shape=plaintext]
_0->0 [label="banana$"]
_2 [shape=point]
_0->_2 [label="na"]
4 [shape=plaintext]
_2->4 [label="$"]
2 [shape=plaintext]
_2->2 [label="na$"]
}
)"},
        {"xabxa$", R"(digraph "xabxa$" {
rankdir=LR
_0 [shape=point]
5 [shape=plaintext]
_0->5 [label="$"]
_2 [shape=point]
_0->_2 [label="a"]
4 [shape=plaintext]
_2->4 [label="$"]
1 [shape=plaintext]
_2->1 [label="bxa$"]
2 [shape=plaintext]
_0->2 [label="bxa$"]
_1 [shape=point]
_0->_1 [label="xa"]
3 [shape=plaintext]
_1->3 [label="$"]
0 [shape=plaintext]
_1->0 [label="bxa$"]
}
)"},
    };
    for (auto& [str, expect] : tests) {
        auto tree = to<suffix_tree_viz>(str);
        check((std::ostringstream{} << tree).str() == expect);
    }
}

void test_longest_common_subsequence_case_insensitive()
{
    log("run");
    struct {
        std::string_view lhs;
        std::string_view rhs;
        std::string_view expect;
    } tests[] = {
        {"XMJYAUZ", "MZJAWXU", "MJAU"},
        {"aggtab", "GXTXAYB", "gtab"},
        {"ABCDGH", "aedfhr", "ADH"},
        {"BANANA", "ATANA", "AANA"},
        {"gac", "AGCAT", "ga"},
        {"BCDAACD", "ACDBAC", "CDAC"},
    };
    for (auto& [lhs, rhs, expect] : tests) {
        auto result = std::string{};
        longest_common_subsequence::copy(
            lhs, rhs, std::back_inserter(result), case_insensitive::equal_to{});
        check(result == expect);
    }
}

void test_longest_common_subsequence_hello_world()
{
    log("run");
    auto result = std::string{};
    longest_common_subsequence::copy("LCS is the basis of "sv,
                                     L"the diff utility"sv,
                                     std::back_inserter(result));
    check(result == "the if ");
}

void test_longest_common_substring_case_insensitive()
{
    log("run");
    auto result = std::string{};
    longest_common_substring::copy("geeksforGeeks"sv,
                                   "GEEKSQUIZ"sv,
                                   std::back_inserter(result),
                                   case_insensitive::less{});
    check(result == "Geeks"sv);
}

void test_longest_common_substring_hello_world()
{
    log("run");
    auto result = std::string{};
    longest_common_substring::copy("not to be confused with longest "sv,
                                   L"common subsequence problem"sv,
                                   std::back_inserter(result));
    check(result == "co"sv);
}

void test_longest_common_substring_find()
{
    log("run");
    struct {
        std::string_view lhs;
        std::string_view rhs;
        std::string_view expect;
    } tests[] = {
        {"xabxac", "abcabxabcd", "abxa"},
        {"xabxaabxa", "babxba", "abx"},
        {"GeeksforGeeks", "GeeksQuiz", "Geeks"},
        {"OldSite:GeeksforGeeks.org", "NewSite:GeeksQuiz.com", "Site:Geeks"},
        {"abcde", "fghie", "e"},
        {"pqrst", "uvwxyz", ""},
    };
    for (auto& [lhs, rhs, expect] : tests) {
        auto result = std::string{};
        longest_common_substring::copy(lhs, rhs, std::back_inserter(result));
        check(expect == result);
    }
}

void test_longest_repeated_substring_case_insensitive()
{
    log("run");
    auto str = "geeksForGeeks$"sv;
    auto arr =
        enhanced_suffix_array<char, unsigned, case_insensitive::less>{str};
    check(longest_repeated_substring(arr) == "Geeks"sv);
    auto tree =
        to<suffix_tree<char,
                       unsigned,
                       std::map<char, unsigned, case_insensitive::less>>>(str);
    check(longest_repeated_substring(tree) == "geeks"sv);
}

void test_longest_repeated_substring_hello_world()
{
    log("run");
    auto arr = enhanced_suffix_array{
        "find the longest substring of a string that occurs at least twice"sv};
    check(longest_repeated_substring(arr) == "string "sv);
}

void test_longest_repeated_substring_find()
{
    log("run");
    struct {
        std::string_view str;
        std::string_view expect;
    } tests[] = {
        {"GEEKSFORGEEKS$", "GEEKS"},
        {"AAAAAAAAAA$", "AAAAAAAAA"},
        {"ABCDEFG$", ""},
        {"ABABABA$", "ABABA"},
        {"ATCGATCGA$", "ATCGA"},
        {"banana$", "ana"},
        {"mississippi$", "issi"},
        {"abcabcaacb$", "abca"},
        {"aababa$", "aba"},
    };
    for (auto& [str, expect] : tests) {
        auto arr = enhanced_suffix_array{str};
        check(longest_repeated_substring(arr) == expect);
        auto tree = to<suffix_tree>(str);
        check(longest_repeated_substring(tree) == expect);
    }
}

void test_substring_search()
{
    log("run");
    struct {
        std::string_view str;
        std::string_view substr;
        std::initializer_list<size_t> expect;
    } tests[] = {
        {"", "", {0}},
        {"abc$", "", {0, 1, 2, 3, 4}},
        {"geeksforgeeks$", "geeks", {0, 8}},
        {"geeksforgeeks$", "geek1", {}},
        {"geeksforgeeks$", "for", {5}},
        {"aabaacaadaabaaabaa$", "aaba", {0, 9, 13}},
        {"aabaacaadaabaaabaa$", "aa", {0, 3, 6, 9, 12, 13, 16}},
        {"aabaacaadaabaaabaa$", "aae", {}},
        {"aaaaaaaaa$", "aaaa", {0, 1, 2, 3, 4, 5}},
        {"aaaaaaaaa$", "aa", {0, 1, 2, 3, 4, 5, 6, 7}},
        {"aaaaaaaaa$", "a", {0, 1, 2, 3, 4, 5, 6, 7, 8}},
        {"aaaaaaaaa$", "ab", {}},
    };
    for (const auto& [str, substr, expect] : tests) {
        auto arr = suffix_array{str};
        check(substring_search::find_any(arr, str) == 0);
        check(!substring_search::find_any(arr, "not found"sv));
        if (auto pos = substring_search::find_any(arr, substr))
            check(std::ranges::find(expect, *pos) != expect.end());
        else
            check(std::ranges::empty(expect));
        check(std::ranges::is_permutation(
            expect,
            to<std::vector>(
                substring_search::find_all(arr, std::string{substr}))));
        auto tree = to<suffix_tree>(str);
        check(substring_search::find_first(tree, str) == 0);
        check(substring_search::find_first(tree, ""sv) == 0);
        check(!substring_search::find_first(tree, "not found"sv));
        if (auto pos = substring_search::find_first(tree, substr))
            check(*expect.begin() == *pos);
        else
            check(std::ranges::empty(expect));
        check(std::ranges::is_permutation(
            expect,
            to<std::vector>(
                substring_search::find_all(tree, std::string{substr}))));
    }
}

void test_suffix_array_hello_world()
{
    log("run");
    auto arr = suffix_array{"space efficient alternative to suffix tree"sv};
    check(substring_search::find_any(arr, "efficient"sv) == 6);
}

void test_suffix_array_lcp()
{
    log("run");
    auto arr = enhanced_suffix_array{"banana"sv};
    auto expect = {1, 3, 0, 0, 2, 0};
    check(std::ranges::equal(arr.lcp_array(), expect));
}

void test_suffix_array_n_tree_cross_check()
{
    log("run");
    auto str = generate_random_string(std::exp2(17));
    str.back() = '$';
    auto arr = suffix_array{str};
    auto tree = to<suffix_tree_viz>(str);
    auto n = 0;
    for (auto edge : tree.depth_first_search(*tree.find(""sv)))
        if (tree.leaf(edge.child_node))
            check(arr[n++] == tree.labels(edge).first);
    check(std::cmp_equal(n, str.size()));
}

void test_suffix_tree_hello_world()
{
    log("run");
    auto tree = to<suffix_tree>("quick search for text within a document"sv);
    check(substring_search::find_first(tree, "text"sv) == 17);
}

int main()
{
    test_edit_distance_case_insensitive();
    test_edit_distance_hello_world();
    test_example_diff();
    test_example_suffix_tree_viz();
    test_longest_common_subsequence_case_insensitive();
    test_longest_common_subsequence_hello_world();
    test_longest_common_substring_case_insensitive();
    test_longest_common_substring_hello_world();
    test_longest_common_substring_find();
    test_longest_repeated_substring_case_insensitive();
    test_longest_repeated_substring_hello_world();
    test_longest_repeated_substring_find();
    test_substring_search();
    test_suffix_array_hello_world();
    test_suffix_array_lcp();
    test_suffix_array_n_tree_cross_check();
    test_suffix_tree_hello_world();
    log("done");
}
