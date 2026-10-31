#include <gtest/gtest.h>
#include "TextAnalyzer.h"
#include "Exceptions.h"
#include <fstream>
#include <cstdio>

using namespace FileCompare;

class TextAnalyzerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        std::ofstream f1("gtest_file1.txt");
        f1 << "hello world\nhello C++ programming language\n";
        f1.close();

        std::ofstream f2("gtest_file2.txt");
        f2 << "hello world\npython programming\n";
        f2.close();

        std::ofstream f3("gtest_empty.txt");
        f3.close();
    }

    void TearDown() override
    {
        std::remove("gtest_file1.txt");
        std::remove("gtest_file2.txt");
        std::remove("gtest_empty.txt");
    }
};

// 1. Успішні базові операції аналізу тексту
TEST_F(TextAnalyzerTest, BasicAnalysisSuccess)
{
    TextAnalyzer a("gtest_file1.txt");
    EXPECT_EQ(a.getLineCount(), 2);
    EXPECT_EQ(a.getTotalWordCount(), 6);
    EXPECT_EQ(a.getWordFrequency("hello"), 2);
    EXPECT_EQ(a.getWordFrequency("world"), 1);
}

// 2. Сценарій помилки 1: Файл відсутній
TEST_F(TextAnalyzerTest, ThrowsFileNotFound)
{
    EXPECT_THROW({
        TextAnalyzer a("non_existent_path.txt");
        }, FileNotFoundException);
}

// 3. Сценарій помилки 3: Порожній набір слів для topWords
TEST_F(TextAnalyzerTest, ThrowsEmptyWordSet)
{
    TextAnalyzer a("gtest_empty.txt");
    EXPECT_THROW({
        (void)a.getTopWords(1);
        }, EmptyWordSetException);
}

// 4. Сценарій помилки 4: Некоректне N для topWords
TEST_F(TextAnalyzerTest, ThrowsInvalidTopN)
{
    TextAnalyzer a("gtest_file1.txt");
    EXPECT_THROW((void)a.getTopWords(0), InvalidTopNException);
    EXPECT_THROW((void)a.getTopWords(-3), InvalidTopNException);
        EXPECT_THROW((void)a.getTopWords(100), InvalidTopNException);
}

// 5. Сценарій помилки 5: Некоректний параметр пошуку
TEST_F(TextAnalyzerTest, ThrowsInvalidSearchQuery)
{
    TextAnalyzer a("gtest_file1.txt");
    EXPECT_THROW((void)a.getWordFrequency(""), InvalidSearchQueryException);
    EXPECT_THROW((void)a.getWordFrequency("...!!!"), InvalidSearchQueryException);
}

// 6. Перевірка Strong Exception Guarantee
TEST_F(TextAnalyzerTest, StrongGuaranteePreservedOnFailedLoad)
{
    TextAnalyzer a("gtest_file1.txt");
    size_t initialLines = a.getLineCount();
    size_t initialWords = a.getTotalWordCount();
    std::string initialPath = a.getLoadedFilePath();

    EXPECT_THROW(a.loadFile("missing_file.txt"), FileNotFoundException);

    EXPECT_EQ(a.getLineCount(), initialLines);
    EXPECT_EQ(a.getTotalWordCount(), initialWords);
    EXPECT_EQ(a.getLoadedFilePath(), initialPath);
}

// 7. Схожість Жаккара (збереження алгоритмів лр4)
TEST_F(TextAnalyzerTest, SimilarityCalculation)
{
    TextAnalyzer a1("gtest_file1.txt");
    TextAnalyzer a2("gtest_file2.txt");

    double sim = a1.similarityWith(a2);
    EXPECT_GT(sim, 0.0);
    EXPECT_LT(sim, 1.0);
}