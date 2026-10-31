#pragma once

#include <stdexcept>
#include <string>

namespace FileCompare
{
    class TextAnalyzerException : public std::runtime_error
    {
    public:
        explicit TextAnalyzerException(const std::string& message)
            : std::runtime_error(message) {}
    };

    class FileNotFoundException : public TextAnalyzerException
    {
    private:
        std::string filePath_;
    public:
        explicit FileNotFoundException(const std::string& path)
            : TextAnalyzerException("Error: File not found at path: '" + path + "'"),
            filePath_(path) {}

        [[nodiscard]] const std::string& getFilePath() const noexcept { return filePath_; }
    };

    class FileReadException : public TextAnalyzerException
    {
    private:
        std::string filePath_;
    public:
        explicit FileReadException(const std::string& path, const std::string& reason)
            : TextAnalyzerException("Error: Failed to read file '" + path + "'. Reason: " + reason),
            filePath_(path) {}

        [[nodiscard]] const std::string& getFilePath() const noexcept { return filePath_; }
    };

    class EmptyWordSetException : public TextAnalyzerException
    {
    public:
        EmptyWordSetException()
            : TextAnalyzerException("Error: Text statistics contain no words to perform topWords query.") {}
    };

    class InvalidTopNException : public TextAnalyzerException
    {
    private:
        int requestedN_;
        size_t availableWords_;
    public:
        InvalidTopNException(int requestedN, size_t availableWords)
            : TextAnalyzerException("Error: Invalid N (" + std::to_string(requestedN) +
                ") for topWords. Available distinct words: " + std::to_string(availableWords) + "."),
            requestedN_(requestedN), availableWords_(availableWords) {}

        [[nodiscard]] int getRequestedN() const noexcept { return requestedN_; }
        [[nodiscard]] size_t getAvailableWords() const noexcept { return availableWords_; }
    };

    class InvalidSearchQueryException : public std::invalid_argument
    {
    private:
        std::string query_;
    public:
        explicit InvalidSearchQueryException(const std::string& query, const std::string& reason)
            : std::invalid_argument("Error: Invalid search query '" + query + "'. Reason: " + reason),
            query_(query) {}

        [[nodiscard]] const std::string& getQuery() const noexcept { return query_; }
    };
}