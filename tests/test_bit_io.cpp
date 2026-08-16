#include "bit_io.h"
#include "test_utils.h"

#include <cstdio>

void test_bit_write_read_roundtrip() {
    const std::string filename = "test_bits.tmp";
    bool bits[] = {true, false, true, true, false, false, true, false, true, true};

    {
        BitWriter writer(filename);
        for (bool b : bits) {
            writer.writeBit(b);
        }
        writer.flush();
    }
    {
        BitReader reader(filename);
        for (bool expected : bits) {
            bool actual;
            CHECK(reader.readBit(actual));
            CHECK(actual == expected);
        }
    }

    std::remove(filename.c_str());
}

void test_byte_and_uint_roundtrip() {
    const std::string filename = "test_bytes.tmp";

    {
        BitWriter writer(filename);
        writer.writeByte(200);
        writer.writeUint16(5000);
        writer.writeUint64(123456789012345ULL);
    }
    {
        BitReader reader(filename);
        CHECK(reader.readByte() == 200);
        CHECK(reader.readUint16() == 5000);
        CHECK(reader.readUint64() == 123456789012345ULL);
    }

    std::remove(filename.c_str());
}

void test_read_past_end_returns_false() {
    const std::string filename = "test_empty.tmp";
    {
        BitWriter writer(filename);
        writer.writeBit(true);
        writer.flush();
    }
    {
        BitReader reader(filename);
        bool bit;
        int successfulReads = 0;
        while (reader.readBit(bit)) {
            successfulReads++;
        }
        // после flush() один бит дополняется нулями до целого байта,
        // поэтому должно быть ровно 8 успешных чтений
        CHECK(successfulReads == 8);
    }
    std::remove(filename.c_str());
}

int main() {
    RUN_TEST(test_bit_write_read_roundtrip);
    RUN_TEST(test_byte_and_uint_roundtrip);
    RUN_TEST(test_read_past_end_returns_false);
    return reportResults();
}
