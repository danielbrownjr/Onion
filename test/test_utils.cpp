#include "gtest/gtest.h"

#include <cstdlib>
#include <cstring>
#include <string>

extern "C" {
#include "../src/common/utils/file.h"
#include "../src/common/utils/str.h"
}

// Copies a string into an exactly-sized heap buffer so AddressSanitizer
// catches any read past the terminator.
static char *exact_copy(const char *s)
{
    size_t n = strlen(s) + 1;
    char *buf = (char *)malloc(n);
    memcpy(buf, s, n);
    return buf;
}

TEST(test_str, trimRemovesSurroundingWhitespace)
{
    char out[STR_MAX];
    char *in = exact_copy("  hello world \n");
    str_trim(out, sizeof(out), in, false);
    EXPECT_STREQ(out, "hello world");
    free(in);
}

TEST(test_str, trimStripsQuotes)
{
    char out[STR_MAX];
    char *in = exact_copy(" \"value\"\n");
    str_trim(out, sizeof(out), in, false);
    EXPECT_STREQ(out, "value");
    free(in);
}

TEST(test_str, trimAllWhitespaceDoesNotOverread)
{
    char out[STR_MAX] = "garbage";
    char *in = exact_copy("   \t\n");
    str_trim(out, sizeof(out), in, false);
    EXPECT_STREQ(out, "");
    free(in);
}

TEST(test_str, trimEmptyString)
{
    char out[STR_MAX] = "garbage";
    char *in = exact_copy("");
    str_trim(out, sizeof(out), in, false);
    EXPECT_STREQ(out, "");
    free(in);
}

TEST(test_str, trimTruncatesToBuffer)
{
    char out[4];
    str_trim(out, sizeof(out), "abcdef", false);
    EXPECT_STREQ(out, "abc");
}

TEST(test_str, countChar)
{
    EXPECT_EQ(str_count_char("a/b/c", '/'), 2);
    EXPECT_EQ(str_count_char("", '/'), 0);
    EXPECT_EQ(str_count_char("abc", '\0'), 0);
}

TEST(test_str, includeCJK)
{
    char latin[] = "Pokemon";
    char accented[] = "Pok\xc3\xa9mon"; // Pokémon
    char japanese[] = "\xe3\x83\x9d\xe3\x82\xb1\xe3\x83\xa2\xe3\x83\xb3"; // ポケモン
    char chinese[] = "\xe6\xb8\xb8\xe6\x88\x8f"; // 游戏
    char korean[] = "\xea\xb2\x8c\xec\x9e\x84"; // 게임
    EXPECT_FALSE(includeCJK(latin));
    EXPECT_FALSE(includeCJK(accented));
    EXPECT_TRUE(includeCJK(japanese));
    EXPECT_TRUE(includeCJK(chinese));
    EXPECT_TRUE(includeCJK(korean));
}

TEST(test_str, endsWith)
{
    EXPECT_TRUE(str_endsWith("game.gba", ".gba"));
    EXPECT_FALSE(str_endsWith("game.gb", ".gba"));
}

TEST(test_file, readMissingFileReturnsNull)
{
    EXPECT_EQ(file_read("./does_not_exist.txt"), (char *)NULL);
}

TEST(test_file, writeMissingFileReturnsFalse)
{
    // O_WRONLY without O_CREAT fails on a missing file
    EXPECT_FALSE(file_write("./does_not_exist_dir/x.txt", "x", 1));
}

TEST(test_file, writeThenRead)
{
    const char *path = "./test_utils_tmp.txt";
    FILE *fp = fopen(path, "w");
    ASSERT_NE(fp, (FILE *)NULL);
    fclose(fp);

    EXPECT_TRUE(file_write(path, "hello", 5));
    char *content = file_read(path);
    ASSERT_NE(content, (char *)NULL);
    EXPECT_STREQ(content, "hello");
    free(content);
    remove(path);
}
