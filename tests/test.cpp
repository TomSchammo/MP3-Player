
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <id3.hpp>


TEST_CASE("Testing convert_bytes from id3.hpp", "[ID3::convert_bytes]") {

    char buffer_1[4] = {0, 0, 1, 63};
    char buffer_2[4] = {0, 0x03, 0x07, 0x76};
    char buffer_3[1] = {static_cast<char>(0xff)};
    char buffer_4[2] = {static_cast<char>(0xff), 0x00};
    // char buffer_5[0] = {};

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

    // SECTION("Testing empty inputs") {
        // REQUIRE(ID3::convert_bytes({}, 0, false) == 0);
        // REQUIRE(ID3::convert_bytes({}, 0, true) == 0);
    // }
}


TEST_CASE("Testing decode_text_retain_position from id3.hpp", "[ID3::decode_text_retain_position]") {

    const std::int8_t ISO = 0x00;
    const std::int8_t UTF16 = 0x01;
    const std::int8_t UTF16B = 0x02;
    const std::int8_t UTF8 = 0x03;

    // 'aA~Ö' starting at position 0, not null terminated
    std::vector<char> ISO1 = {0x61, 0x41, 0x7e, (char)0xd6};

    // 'aA~' starting at position 1, not null terminated
    std::vector<char> ISO2 = {0x00, 0x61, 0x41, 0x7e};

    // 'a', starting at position 2, null terminated
    std::vector<char> ISO3 = {0x00, 0x00, 0x61, 0x00, 0x00};

    // 'A' starting at position 2, null terminated
    std::vector<char> ISO4 = {0x00, 0x61, 0x41, 0x00, 0x7e};

    // 'Ö', not null terminated
    std::vector<char> ISO5 = {(char)0xd6};

    SECTION("Testing ISO-8859-1 implementation") {
        auto container1 = ID3::decode_text_retain_position(ISO, ISO1, 0);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~Ö");
        REQUIRE(container1.error == false);

        auto container2 = ID3::decode_text_retain_position(ISO, ISO2, 1);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~");
        REQUIRE(container1.error == false);

        auto container3 = ID3::decode_text_retain_position(ISO, ISO3, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "a");
        REQUIRE(container1.error == false);

        auto container4 = ID3::decode_text_retain_position(ISO, ISO4, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "A");
        REQUIRE(container1.error == false);

        auto container5 = ID3::decode_text_retain_position(ISO, ISO5, 0);

        REQUIRE(container1.position == 1);
        REQUIRE(container1.text == "Ö");
        REQUIRE(container1.error == false);
    }


    // 'aA~Ö' starting at position 0, not null terminated TODO
    std::vector<char> UTF16_1 = {0x61, 0x41, 0x7e, (char)0xc3, (char)0x96};

    // 'aA~' starting at position 1, not null terminated TODO
    std::vector<char> UTF16_2 = {0x00, 0x61, 0x41, 0x7e};

    // 'a', starting at position 2, null terminated TODO
    std::vector<char> UTF16_3 = {0x00, 0x00, 0x61, 0x00, 0x00};

    // 'A' starting at position 2, null terminated TODO
    std::vector<char> UTF16_4 = {0x00, 0x61, 0x41, 0x00, 0x7e};

    // 'Ö', not null terminated TODO
    std::vector<char> UTF16_5 = {(char)0xc3, (char)0x96};


    SECTION("Testing UTF-16 with BOM implementation") {
        // TODO

        auto container1 = ID3::decode_text_retain_position(UTF16, UTF16_1, 0);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~Ö");
        REQUIRE(container1.error == false);

        auto container2 = ID3::decode_text_retain_position(UTF16, UTF16_2, 1);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~");
        REQUIRE(container1.error == false);

        auto container3 = ID3::decode_text_retain_position(UTF16, UTF16_3, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "a");
        REQUIRE(container1.error == false);

        auto container4 = ID3::decode_text_retain_position(UTF16, UTF16_4, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "A");
        REQUIRE(container1.error == false);

        auto container5 = ID3::decode_text_retain_position(UTF16, UTF16_5, 0);

        REQUIRE(container1.position == 1);
        REQUIRE(container1.text == "Ö");
        REQUIRE(container1.error == false);
    }


    // 'aA~Ö' starting at position 0, not null terminated TODO
    std::vector<char> UTF16B_1 = {0x61, 0x41, 0x7e, (char)0xc3, (char)0x96};

    // 'aA~' starting at position 1, not null terminated TODO
    std::vector<char> UTF16B_2 = {0x00, 0x61, 0x41, 0x7e};

    // 'a', starting at position 2, null terminated TODO
    std::vector<char> UTF16B_3 = {0x00, 0x00, 0x61, 0x00, 0x00};

    // 'A' starting at position 2, null terminated TODO
    std::vector<char> UTF16B_4 = {0x00, 0x61, 0x41, 0x00, 0x7e};

    // 'Ö', not null terminated TODO
    std::vector<char> UTF16B_5 = {(char)0xc3, (char)0x96};


    SECTION("Testing UTF-16B implementation") {
        // TODO

        auto container1 = ID3::decode_text_retain_position(UTF16B, UTF16B_1, 0);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~Ö");
        REQUIRE(container1.error == false);

        auto container2 = ID3::decode_text_retain_position(UTF16B, UTF16B_2, 1);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~");
        REQUIRE(container1.error == false);

        auto container3 = ID3::decode_text_retain_position(UTF16B, UTF16B_3, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "a");
        REQUIRE(container1.error == false);

        auto container4 = ID3::decode_text_retain_position(UTF16B, UTF16B_4, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "A");
        REQUIRE(container1.error == false);

        auto container5 = ID3::decode_text_retain_position(UTF16B, UTF16B_5, 0);

        REQUIRE(container1.position == 1);
        REQUIRE(container1.text == "Ö");
        REQUIRE(container1.error == false);
    }


    // 'aA~Ö' starting at position 0, not null terminated
    std::vector<char> UTF8_1 = {0x61, 0x41, 0x7e, (char)0xc3, (char)0x96};

    // 'aA~' starting at position 1, not null terminated
    std::vector<char> UTF8_2 = {0x00, 0x61, 0x41, 0x7e};

    // 'a', starting at position 2, null terminated
    std::vector<char> UTF8_3 = {0x00, 0x00, 0x61, 0x00, 0x00};

    // 'A' starting at position 2, null terminated
    std::vector<char> UTF8_4 = {0x00, 0x61, 0x41, 0x00, 0x7e};

    // 'Ö', not null terminated
    std::vector<char> UTF8_5 = {(char)0xc3, (char)0x96};


    SECTION("Testing UTF-8 implementation") {
        auto container1 = ID3::decode_text_retain_position(UTF8, UTF8_1, 0);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~Ö");
        REQUIRE(container1.error == false);

        auto container2 = ID3::decode_text_retain_position(UTF8, UTF8_2, 1);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "aA~");
        REQUIRE(container1.error == false);

        auto container3 = ID3::decode_text_retain_position(UTF8, UTF8_3, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "a");
        REQUIRE(container1.error == false);

        auto container4 = ID3::decode_text_retain_position(UTF8, UTF8_4, 2);

        REQUIRE(container1.position == 4);
        REQUIRE(container1.text == "A");
        REQUIRE(container1.error == false);

        auto container5 = ID3::decode_text_retain_position(UTF8, UTF8_5, 0);

        REQUIRE(container1.position == 1);
        REQUIRE(container1.text == "Ö");
        REQUIRE(container1.error == false);
    }

    SECTION("Testing error handling") {
        // TODO test for invalid encoding
        // TODO test for empty data
        // TODO test for position out of bounds
        // TODO test for negative position
    }
}


TEST_CASE("Testing the synchronize function from id3.hpp", "[ID3::synchronize]") {


    SECTION("Testing empty input buffers") {

        std::vector<char> test_data_1 = {};
        ID3::synchronize(test_data_1);
        REQUIRE(test_data_1.size() == 0);
    }


    SECTION("Testing buffers with no unsynchronization") {

        std::vector<char> test_data_1 =  {0x00, (char)0xff, 0x4e, (char)0xff, (char)0xbe};
        ID3::synchronize(test_data_1);
        REQUIRE(test_data_1.size() == 5);

        std::vector<char> test_data_2 =  {0x01, (char)0xff, 0x4e, (char)0xff, (char)0xbe};
        ID3::synchronize(test_data_2);
        REQUIRE(test_data_2.size() == 5);

        std::vector<char> test_data_3 =  {0x00, (char)0xfe, 0x4e, (char)0xfc, (char)0xbe};
        ID3::synchronize(test_data_3);
        REQUIRE(test_data_3.size() == 5);
    }


    SECTION("Testing buffers with unsynchronization") {

        std::vector<char> test_data_1 =  {(char)0xff, 0x00, (char)0xff, (char)0xbe};
        ID3::synchronize(test_data_1);
        REQUIRE(test_data_1.size() == 3);

        std::vector<char> test_data_2 =  {0x00, (char)0xff, 0x00, (char)0xff, (char)0xbe};
        ID3::synchronize(test_data_2);
        REQUIRE(test_data_2.size() == 4);

        std::vector<char> test_data_3 =  {(char)0xff, 0x00, (char)0xff, 0x00, (char)0xbe, 0x00};
        ID3::synchronize(test_data_3);
        REQUIRE(test_data_3.size() == 4);
    }

}


TEST_CASE("Testing the convert_size function from id3.hpp", "[convert_size]") {


    SECTION("Testing too big integers") {


        SECTION("Testing 0xffffffff...") {

            std::uint32_t integer = 0xffffffff;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(arr.at(0) == 0xde);
            REQUIRE(arr.at(1) == 0xad);
            REQUIRE(arr.at(2) == 0xbe);
            REQUIRE(arr.at(3) == 0xef);
        }


        SECTION("Testing 0x7f7f7f80...") {

            std::uint32_t integer = 0x7f7f7f80;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(arr.at(0) == 0xde);
            REQUIRE(arr.at(1) == 0xad);
            REQUIRE(arr.at(2) == 0xbe);
            REQUIRE(arr.at(3) == 0xef);
        }


        SECTION("Testing 0x807f7f80...") {

            std::uint32_t integer = 0x807f7f7f;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(arr.at(0) == 0xde);
            REQUIRE(arr.at(1) == 0xad);
            REQUIRE(arr.at(2) == 0xbe);
            REQUIRE(arr.at(3) == 0xef);
        }

    }


    SECTION("Testing integers that need 1 byte") {


        SECTION("Testing 0x00...") {

            std::uint32_t integer = 0x00;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x7f...") {

            std::uint32_t integer = 0x7f;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x70...") {

            std::uint32_t integer = 0x70;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x40...") {

            std::uint32_t integer = 0x40;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


    }


    SECTION("Testing integers that need 2 bytes") {


        SECTION("Testing 0x7f7f...") {


            std::uint32_t integer = 0x7f7f;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x0100...") {


            std::uint32_t integer = 0x0100;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x7000...") {


            std::uint32_t integer = 0x7000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x4000...") {


            std::uint32_t integer = 0x4000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0xff...") {


            std::uint32_t integer = 0xff;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


    }


    SECTION("Testing integers that need 3 bytes") {


        SECTION("Testing 0x7f7f7f...") {


            std::uint32_t integer = 0x7f7f7f;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x010000...") {


            std::uint32_t integer = 0x010000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x700000...") {


            std::uint32_t integer = 0x700000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0x400000...") {


            std::uint32_t integer = 0x400000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


        SECTION("Testing 0xffff...") {


            std::uint32_t integer = 0xffff;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();


        }


    }


    SECTION("Testing integers that need 4 bytes") {


        SECTION("Testing 0x7f7f7f7f...") {


            std::uint32_t integer = 0x7f7f7f7f;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();

        }


        SECTION("Testing 0x01000000...") {


            std::uint32_t integer = 0x01000000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();

        }


        SECTION("Testing 0x70000000...") {


            std::uint32_t integer = 0x70000000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();

        }


        SECTION("Testing 0x40000000...") {


            std::uint32_t integer = 0x40000000;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();

        }


        SECTION("Testing 0xffffff...") {


            std::uint32_t integer = 0xffffff;
            std::array<std::uint8_t, 4> arr {0xde, 0xad, 0xbe, 0xef};

            convert_size(integer, arr);

            REQUIRE(false);
            // REQUIRE();
            // REQUIRE();
            // REQUIRE();

        }


    }

}


// TEST_CASE("Testing the convert_dec function from id3.hpp", "[convert_dec]") {
//
// }


