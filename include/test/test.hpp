
#include <catch2/catch.hpp>
#include <id3.hpp>

TEST_CASE("Testing convert_bytes from id3.hpp", "[ID3::convert_bytes]") {

    char buffer_1[4] = {0, 0, 1, 63};
    char buffer_2[4] = {0, 0x03, 0x07, 0x76};
    char buffer_3[1] = {static_cast<char>(0xff)};
    char buffer_4[2] = {static_cast<char>(0xff), 0x00};

    SECTION("Testing conversion of non syncsafe integers") {
        REQUIRE(ID3::convert_bytes(buffer_1, 4, false) == 319);
        REQUIRE(ID3::convert_bytes(buffer_2, 4, false) == 198518);
        REQUIRE(ID3::convert_bytes(buffer_3, 1, false) == 255);
        REQUIRE(ID3::convert_bytes(buffer_4, 2, false) == 65280);
    }

    SECTION("Testing conversion of syncsafe integers") {
        REQUIRE(ID3::convert_bytes(buffer_1, 4, true) == 191);
        REQUIRE(ID3::convert_bytes(buffer_2, 4, true) == 50166);
    }

    // TODO there is an issue when passing non syncsafe integers as syncsafe intgers
    SECTION("Testing supplying a non syncsafe integer as a syncsafe integer") {
        REQUIRE(ID3::convert_bytes(buffer_4, 2, true) == 16256);
        REQUIRE(ID3::convert_bytes(buffer_3, 1, true) == 127);
    }
}

