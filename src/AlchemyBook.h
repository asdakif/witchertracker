#pragma once
#include <string>
#include <map>

class AlchemyBook {
private:
    std::map<std::string, std::map<std::string, int>> formulas;

public:
    bool addFormula(const std::string& potion, const std::map<std::string, int>& ingredients) {
        if (formulas.count(potion)) return false;
        formulas[potion] = ingredients;
        return true;
    }

    bool hasFormula(const std::string& potion) const {
        return formulas.count(potion);
    }

    std::map<std::string, int> getFormula(const std::string& potion) const {
        auto it = formulas.find(potion);
        if (it != formulas.end()) return it->second;
        return {};
    }
};
