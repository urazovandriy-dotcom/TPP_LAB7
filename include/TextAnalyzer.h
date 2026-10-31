#pragma once

#include "Exceptions.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace FileCompare
{
    class TextAnalyzer
    {
    public:
        TextAnalyzer() noexcept;
        explicit TextAnalyzer(const std::string& filePath);

        TextAnalyzer(const TextAnalyzer& other) = default;
        TextAnalyzer(TextAnalyzer&& other) noexcept = default;
        TextAnalyzer& operator=(const TextAnalyzer& other);
        TextAnalyzer& operator=(TextAnalyzer&& other) noexcept;
        ~TextAnalyzer() noexcept = default;

        void swap(TextAnalyzer& other) noexcept;

        // Strong Exception Guarantee
        void loadFile(const std::string& filePath);

        [[nodiscard]] const std::string& getLoadedFilePath() const noexcept;
        [[nodiscard]] size_t getLineCount() const noexcept;
        [[nodiscard]] size_t getTotalWordCount() const noexcept;
        [[nodiscard]] size_t getUniqueWordCount() const noexcept;
        [[nodiscard]] bool isEmpty() const noexcept;

        [[nodiscard]] size_t getWordFrequency(const std::string& word) const;
        [[nodiscard]] std::vector<std::pair<std::string, size_t>> getTopWords(int n) const;

        [[nodiscard]] double similarityWith(const TextAnalyzer& other) const noexcept;
        [[nodiscard]] std::vector<std::string> commonWordsWith(const TextAnalyzer& other) const;
        [[nodiscard]] int differenceCountWith(const TextAnalyzer& other) const noexcept;

    private:
        std::string loadedFilePath_;
        std::vector<std::string> lines_;
        std::unordered_map<std::string, size_t> wordFrequency_;
        std::unordered_set<std::string> uniqueWords_;
        size_t totalWords_{ 0 };

        static void parseContent(const std::string& path,
            std::vector<std::string>& outLines,
            std::unordered_map<std::string, size_t>& outFreq,
            std::unordered_set<std::string>& outUnique,
            size_t& outTotalWords);

        static std::string normalizeWord(const std::string& raw);
    };
}