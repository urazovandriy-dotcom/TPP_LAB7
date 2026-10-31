#include "TextAnalyzer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace FileCompare
{
    TextAnalyzer::TextAnalyzer() noexcept = default;

    TextAnalyzer::TextAnalyzer(const std::string& filePath)
    {
        loadFile(filePath);
    }

    // Патерн copy-and-swap
    TextAnalyzer& TextAnalyzer::operator=(const TextAnalyzer& other)
    {
        if (this != &other)
        {
            TextAnalyzer temp(other);
            this->swap(temp);
        }
        return *this;
    }

    TextAnalyzer& TextAnalyzer::operator=(TextAnalyzer&& other) noexcept
    {
        if (this != &other)
        {
            this->swap(other);
        }
        return *this;
    }

    void TextAnalyzer::swap(TextAnalyzer& other) noexcept
    {
        using std::swap;
        swap(loadedFilePath_, other.loadedFilePath_);
        swap(lines_, other.lines_);
        swap(wordFrequency_, other.wordFrequency_);
        swap(uniqueWords_, other.uniqueWords_);
        swap(totalWords_, other.totalWords_);
    }

    std::string TextAnalyzer::normalizeWord(const std::string& raw)
    {
        std::string cleaned;
        for (char ch : raw)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)))
            {
                cleaned += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
        }
        return cleaned;
    }

    void TextAnalyzer::parseContent(const std::string& path,
        std::vector<std::string>& outLines,
        std::unordered_map<std::string, size_t>& outFreq,
        std::unordered_set<std::string>& outUnique,
        size_t& outTotalWords)
    {
        // Сценарій 1: Файл відсутній
        if (!std::filesystem::exists(path))
        {
            throw FileNotFoundException(path);
        }

        std::ifstream file(path);
        // Сценарій 2: Неможливість читання або блокування файлу
        if (!file.is_open())
        {
            throw FileReadException(path, "Permission denied or file locked.");
        }

        std::string line;
        while (std::getline(file, line))
        {
            outLines.push_back(line);
            std::stringstream ss(line);
            std::string token;
            while (ss >> token)
            {
                std::string cleaned = normalizeWord(token);
                if (!cleaned.empty())
                {
                    outFreq[cleaned]++;
                    outUnique.insert(cleaned);
                    outTotalWords++;
                }
            }
        }

        if (file.bad())
        {
            throw FileReadException(path, "Stream read error (badbit).");
        }
    }

    // Strong Exception Guarantee: стан не змінюється до успішного завершення
    void TextAnalyzer::loadFile(const std::string& filePath)
    {
        std::vector<std::string> tempLines;
        std::unordered_map<std::string, size_t> tempFreq;
        std::unordered_set<std::string> tempUnique;
        size_t tempTotalWords = 0;

        // Виконання операції над тимчасовими об'єктами
        parseContent(filePath, tempLines, tempFreq, tempUnique, tempTotalWords);

        // Фіксація змін без винятків
        loadedFilePath_ = filePath;
        lines_ = std::move(tempLines);
        wordFrequency_ = std::move(tempFreq);
        uniqueWords_ = std::move(tempUnique);
        totalWords_ = tempTotalWords;
    }

    const std::string& TextAnalyzer::getLoadedFilePath() const noexcept
    {
        return loadedFilePath_;
    }

    size_t TextAnalyzer::getLineCount() const noexcept
    {
        return lines_.size();
    }

    size_t TextAnalyzer::getTotalWordCount() const noexcept
    {
        return totalWords_;
    }

    size_t TextAnalyzer::getUniqueWordCount() const noexcept
    {
        return uniqueWords_.size();
    }

    bool TextAnalyzer::isEmpty() const noexcept
    {
        return lines_.empty() && totalWords_ == 0;
    }

    // Сценарій 5: Некоректний параметр пошуку
    size_t TextAnalyzer::getWordFrequency(const std::string& word) const
    {
        if (word.empty())
        {
            throw InvalidSearchQueryException(word, "Search query string is empty.");
        }

        std::string cleaned = normalizeWord(word);
        if (cleaned.empty())
        {
            throw InvalidSearchQueryException(word, "Search query contains no alphanumeric characters.");
        }

        auto it = wordFrequency_.find(cleaned);
        return (it != wordFrequency_.end()) ? it->second : 0;
    }

    // Сценарії 3 та 4: Порожній набір слів або некоректне N
    std::vector<std::pair<std::string, size_t>> TextAnalyzer::getTopWords(int n) const
    {
        // Сценарій 3: Порожній набір слів
        if (uniqueWords_.empty())
        {
            throw EmptyWordSetException();
        }

        // Сценарій 4: Некоректне N для topWords
        if (n <= 0 || static_cast<size_t>(n) > uniqueWords_.size())
        {
            throw InvalidTopNException(n, uniqueWords_.size());
        }

        std::vector<std::pair<std::string, size_t>> sortedWords(wordFrequency_.begin(), wordFrequency_.end());
        std::sort(sortedWords.begin(), sortedWords.end(), [](const auto& a, const auto& b) {
            if (a.second != b.second)
            {
                return a.second > b.second;
            }
            return a.first < b.first;
            });

        sortedWords.resize(n);
        return sortedWords;
    }

    // Збереження функціональності порівняння
    double TextAnalyzer::similarityWith(const TextAnalyzer& other) const noexcept
    {
        if (uniqueWords_.empty() && other.uniqueWords_.empty())
        {
            return 1.0;
        }
        if (uniqueWords_.empty() || other.uniqueWords_.empty())
        {
            return 0.0;
        }

        size_t intersectionCount = 0;
        for (const auto& w : uniqueWords_)
        {
            if (other.uniqueWords_.find(w) != other.uniqueWords_.end())
            {
                intersectionCount++;
            }
        }

        size_t unionCount = uniqueWords_.size() + other.uniqueWords_.size() - intersectionCount;
        return unionCount == 0 ? 0.0 : (static_cast<double>(intersectionCount) / unionCount);
    }

    std::vector<std::string> TextAnalyzer::commonWordsWith(const TextAnalyzer& other) const
    {
        std::vector<std::string> common;
        for (const auto& w : uniqueWords_)
        {
            if (other.uniqueWords_.find(w) != other.uniqueWords_.end())
            {
                common.push_back(w);
            }
        }
        std::sort(common.begin(), common.end());
        return common;
    }

    int TextAnalyzer::differenceCountWith(const TextAnalyzer& other) const noexcept
    {
        int diff = 0;
        size_t minSize = std::min(lines_.size(), other.lines_.size());

        for (size_t i = 0; i < minSize; ++i)
        {
            if (lines_[i] != other.lines_[i])
            {
                diff++;
            }
        }

        diff += static_cast<int>(std::max(lines_.size(), other.lines_.size()) - minSize);
        return diff;
    }
}