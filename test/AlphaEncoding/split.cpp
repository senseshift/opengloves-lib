#include <catch2/catch_all.hpp>

#include <opengloves.hpp>
#include <opengloves/alpha.hpp>

using namespace opengloves;

void check(std::string string, std::map<std::string, std::string> expected) {
  std::map<std::string, std::string> split;

  AlphaEncoding::splitPairs(string.c_str(), string.size(), split);

  REQUIRE(split == expected);
}

TEST_CASE("AlphaEncoding::splitPairs", "[alpha]") {
  check("A0B0C0D0E0\n", {
    {"A", "0"},
    {"B", "0"},
    {"C", "0"},
    {"D", "0"},
    {"E", "0"},
  });

  check("A2047B2047C2047D2047E2047\n", {
    {"A", "2047"},
    {"B", "2047"},
    {"C", "2047"},
    {"D", "2047"},
    {"E", "2047"},
  });

  check("A100(AB)200B300(BB)400C500\n", {
    { "A", "100" },
    { "(AB)", "200" },
    { "B", "300" },
    { "(BB)", "400" },
    { "C", "500" },
  });
}