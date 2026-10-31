#include <iostream>
#include <string>
#include <exception>

void throwPrimitiveInt()
{
    std::cout << "Throwing primitive int: 404\n";
    throw 404; // виняток примітивного типу
}

void throwPrimitiveString()
{
    std::cout << "Throwing std::string directly\n";
    throw std::string("Critical raw string error"); // не успадковано від std::exception
}

int main()
{
    std::cout << "=== DEMO: Bad Exception Handling Practices ===\n\n";

    // 1. Тест винятку типу int
    try
    {
        throwPrimitiveInt();
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught via std::exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Caught via catch(...)! std::exception handler MISSED the int exception.\n\n";
    }

    // 2. Тест винятку типу std::string
    try
    {
        throwPrimitiveString();
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught via std::exception: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Caught via catch(...)! std::exception handler MISSED the std::string exception.\n\n";
    }

    return 0;
}