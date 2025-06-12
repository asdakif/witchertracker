#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

class Inventory {
private:
    std::map<std::string, int> ingredients;
    std::map<std::string, int> potions;
    std::map<std::string, int> trophies;

public:
    void addIngredient(const std::string& name, int qty) {
        ingredients[name] += qty;
    }
     bool hasIngredient(const std::string& name) const {
        return ingredients.count(name) > 0 && ingredients.at(name) > 0;
    }

    bool hasPotion(const std::string& name) const {
        return potions.count(name) > 0 && potions.at(name) > 0;
    }

    bool hasTrophy(const std::string& name) const {
        return trophies.count(name) > 0 && trophies.at(name) > 0;
    }

    bool useIngredients(const std::map<std::string, int>& recipe) {
        for (const auto& pair : recipe) {
            if (ingredients[pair.first] < pair.second) return false;
        }
        for (const auto& pair : recipe) {
            ingredients[pair.first] -= pair.second;
        }
        return true;
    }

    void addPotion(const std::string& name) {
        potions[name]++;
    }

    bool consumePotion(const std::string& name) {
        if (potions[name] == 0) return false;
        potions[name]--;
        return true;
    }

    void addTrophy(const std::string& monster) {
        trophies[monster]++;
    }

    bool tradeTrophies(const std::map<std::string, int>& required) {
        for (const auto& pair : required) {
            if (trophies[pair.first] < pair.second) return false;
        }
        for (const auto& pair : required) {
            trophies[pair.first] -= pair.second;
        }
        return true;
    }

    int getIngredient(const std::string& name) const {
        auto it = ingredients.find(name);
        return it != ingredients.end() ? it->second : 0;
    }

    int getPotion(const std::string& name) const {
        auto it = potions.find(name);
        return it != potions.end() ? it->second : 0;
    }

    int getTrophy(const std::string& name) const {
        auto it = trophies.find(name);
        return it != trophies.end() ? it->second : 0;
    }

    std::map<std::string, int> getAllIngredients() const { return ingredients; }
    std::map<std::string, int> getAllPotions() const { return potions; }
    std::map<std::string, int> getAllTrophies() const { return trophies; }
};
