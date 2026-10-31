#include "TextAnalyzer.h"
#include "ScopeLogger.h"
#include <iostream>
#include <fstream>

using namespace FileCompare;

// Рівень 1: виникнення винятку
void lowLevelTask(const std::string& path)
{
    ScopeLogger logger("lowLevelTask()");
    TextAnalyzer analyzer;
    analyzer.loadFile(path); // Викине FileNotFoundException
}

// Рівень 2: перехоплення, вивід діагностики та rethrow (throw;)
void midLevelTask(const std::string& path)
{
    ScopeLogger logger("midLevelTask()");
    try
    {
        lowLevelTask(path);
    }
    catch (const TextAnalyzerException& ex)
    {
        std::cout << "  [midLevelTask DIAGNOSTIC] Intercepted: " << ex.what() << '\n';
        std::cout << "  [midLevelTask DIAGNOSTIC] Rethrowing...\n";
        throw; // Повторна генерація винятку
    }
}

// Рівень 3: виклик проміжного рівня
void highLevelTask(const std::string& path)
{
    ScopeLogger logger("highLevelTask()");
    midLevelTask(path);
}

int main()
{
    std::cout << "=== DEMO 1: Stack Unwinding, ScopeLogger & Rethrow ===\n\n";

    try
    {
        highLevelTask("non_existent_file.txt");
    }
    // Перехоплення від найбільш спеціалізованого типу до базового
    catch (const FileNotFoundException& ex)
    {
        std::cout << "\n[main HANDLER] Caught FileNotFoundException: " << ex.what() << '\n';
        std::cout << "[main HANDLER] Missing path: " << ex.getFilePath() << "\n\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "\n[main HANDLER] Caught std::exception: " << ex.what() << "\n\n";
    }

    std::cout << "=== DEMO 2: Strong Guarantee Verification ===\n";

    const std::string validFileName = "test_valid_demo.txt";
    {
        std::ofstream validFile(validFileName);
        validFile << "apple banana cherry apple\nsecond line of text";
    }

    TextAnalyzer analyzer(validFileName);
    std::cout << "Initial file:  " << analyzer.getLoadedFilePath() << "\n";
    std::cout << "Initial words: " << analyzer.getTotalWordCount() << "\n\n";

    // Спроба виконати операцію з помилкою
    try
    {
        analyzer.loadFile("corrupted_file_demo.txt");
    }
    catch (const TextAnalyzerException& e)
    {
        std::cout << "Caught expected: " << e.what() << "\n";
    }

    // Перевірка інваріанта: стан об'єкта не змінився
    std::cout << "Current file:  " << analyzer.getLoadedFilePath() << "\n";
    std::cout << "Current words: " << analyzer.getTotalWordCount() << "\n";

    if (analyzer.getLoadedFilePath() == validFileName && analyzer.getTotalWordCount() == 6)
    {
        std::cout << ">>> RESULT: Strong Guarantee Preserved! <<<\n";
    }

    return 0;
}