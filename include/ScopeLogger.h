#pragma once

#include <iostream>
#include <string>

namespace FileCompare
{
    class ScopeLogger
    {
    private:
        std::string scopeName_;
    public:
        explicit ScopeLogger(std::string name)
            : scopeName_(std::move(name))
        {
            std::cout << "[ScopeLogger] ENTER: " << scopeName_ << '\n';
        }

        ~ScopeLogger() noexcept
        {
            std::cout << "[ScopeLogger] EXIT : " << scopeName_ << '\n';
        }
    };
}