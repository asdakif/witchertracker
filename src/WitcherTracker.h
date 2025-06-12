// === WitcherTracker.h ===
#pragma once
#include "Inventory.h"
#include "AlchemyBook.h"
#include "Bestiary.h"
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

/**
 * @class WitcherTracker
 * @brief Main interface class that handles input parsing and game logic for Geralt's alchemical inventory, formulas, and bestiary.
 */
class WitcherTracker {
private:
    Inventory inventory;     ///< Manages Geralt's ingredients, potions, and trophies.
    AlchemyBook alchemy;     ///< Stores known potion formulas.
    Bestiary bestiary;       ///< Records effectiveness of items against monsters.

    /**
     * @brief Normalize string: trims extra spaces and converts to lowercase.
     * @param s Input string.
     * @return Normalized string.
     */
    static std::string normalize(const std::string& s) {
        std::istringstream iss(s);
        std::string token, res;
        while (iss >> token) {
            if (!res.empty()) res += ' ';
            res += token;
        }
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
        return res;
    }

    /**
     * @brief Remove leading/trailing spaces and collapse multiple spaces to one.
     * @param s Input string.
     * @return Cleaned string.
     */
    static std::string strip(const std::string& s) {
        std::string out = std::regex_replace(s, std::regex(R"(\s{2,})"), " ");
        return std::regex_replace(out, std::regex(R"(^\s+|\s+$)"), "");
    }

public:
    /**
     * @brief Starts the main input loop and handles commands until 'exit'.
     */
    void run() {
        std::string line;
        while (true) {
            std::cout << ">> ";
            if (!std::getline(std::cin, line)) break;
            if (normalize(line) == "exit") break;
            handleCommand(line);
        }
    }

    /**
     * @brief Parses and processes a single command line input.
     * @param line Input command string.
     */
    void handleCommand(const std::string& line) {
        // Early rejection of invalid characters
        for (size_t i = 0; i + 1 < line.size(); ++i) {
            char c = line[i];
            if (c == '*' || c == '!' || c == '?') {
                std::cout << "INVALID" << std::endl;
                return;
            }
        }

        std::smatch match;
        std::string lowerLine = normalize(line);

        // Handle loot input
        std::regex lootRe(R"(.*loots\s+(.+))");
        if (std::regex_match(line, match, lootRe)) {
            std::string lootList = match[1];
            std::regex fullLootRe(R"(^\s*(\d+\s+[A-Za-z0-9_]+)(\s*,\s*\d+\s+[A-Za-z0-9_]+)*\s*$)");
            if (!std::regex_match(lootList, fullLootRe)) {
                std::cout << "INVALID" << std::endl;
                return;
            }
            std::regex pairRe(R"((\d+)\s+([A-Za-z0-9_]+))");
            std::sregex_iterator it(lootList.begin(), lootList.end(), pairRe), end;
            for (; it != end; ++it) {
                int qty = std::stoi((*it)[1]);
                if (qty <= 0) {
                    std::cout << "INVALID" << std::endl;
                    return;
                }
                inventory.addIngredient((*it)[2], qty);
            }
            std::cout << "Alchemy ingredients obtained" << std::endl;
            return;
        }

        // Handle learning new potion formulas
        std::regex formulaRe(R"(.*learns\s+(.+?)\s+potion\s+consists\s+of\s+(.+))", std::regex::icase);
        if (std::regex_match(line, match, formulaRe)) {
            std::string potion = strip(match[1]);
            std::string contents = match[2];
            std::regex pairRe(R"((\d+)\s+([A-Za-z0-9_]+))");
            std::sregex_iterator it(contents.begin(), contents.end(), pairRe), end;
            std::map<std::string, int> ing;
            for (; it != end; ++it) {
                int qty = std::stoi((*it)[1]);
                if (qty <= 0) { std::cout << "INVALID" << std::endl; return; }
                ing[(*it)[2]] += qty;
            }
            if (ing.empty()) { std::cout << "INVALID" << std::endl; return; }
            std::string normPotion = normalize(potion);
            if (!alchemy.addFormula(normPotion, ing)) {
                std::cout << "Formula already exists: " << potion << std::endl;
            } else {
                std::cout << "New alchemy formula obtained: " << potion << std::endl;
            }
            return;
        }

        // Handle brewing potions
        std::regex brewRe(R"(.*brews\s+(.+))");
        if (std::regex_match(line, match, brewRe)) {
            std::string potion = strip(match[1]);
            std::string normPotion = normalize(potion);
            if (!alchemy.hasFormula(normPotion)) {
                std::cout << "No formula for " << potion << std::endl;
            } else {
                auto recipe = alchemy.getFormula(normPotion);
                if (inventory.useIngredients(recipe)) {
                    inventory.addPotion(potion);
                    std::cout << "Alchemy item created: " << potion << std::endl;
                } else {
                    std::cout << "Not enough ingredients" << std::endl;
                }
            }
            return;
        }

        // Handle bestiary knowledge addition
        std::regex learnRe(R"(.*learns\s+(.+?)\s+is\s+effective\s+against\s+(.+))", std::regex::icase);
        if (std::regex_match(line, match, learnRe)) {
            std::string item = strip(match[1]);
            std::string monster = strip(match[2]);
            if (item.size() > 5 && item.substr(item.size() - 5) == " sign")
                item = item.substr(0, item.size() - 5);
            std::cout << bestiary.addEffectiveness(monster, item) << std::endl;
            return;
        }

        // Handle monster encounters
        std::regex encounterRe(R"(.*encounters\s+a\s+([A-Za-z0-9_]+))");
        if (std::regex_match(line, match, encounterRe)) {
            std::string monster = match[1];
            if (!bestiary.hasEffectiveAgainst(monster)) {
                std::cout << "Geralt is unprepared and barely escapes with his life" << std::endl;
            } else {
                inventory.addTrophy(monster);
                std::cout << "Geralt defeats " << monster << std::endl;
            }
            return;
        }

        // Handle trades
        std::regex tradeRe(R"(.*trades\s+(.+?)\s+trophy\s+for\s+(.+))", std::regex::icase);
        if (std::regex_match(line, match, tradeRe)) {
            std::string trophiesList = match[1];
            std::string ingredientsList = match[2];
            std::regex pairRe(R"((\d+)\s+([A-Za-z0-9_]+))");

            std::map<std::string, int> trophies;
            for (auto it = std::sregex_iterator(trophiesList.begin(), trophiesList.end(), pairRe); it != std::sregex_iterator(); ++it) {
                int qty = std::stoi((*it)[1]);
                if (qty <= 0) { std::cout << "Not enough trophies" << std::endl; return; }
                trophies[(*it)[2]] += qty;
            }

            for (const auto& [monster, qty] : trophies) {
                if (inventory.getTrophy(monster) < qty) {
                    std::cout << "Not enough trophies" << std::endl;
                    return;
                }
            }

            std::map<std::string, int> ingredients;
            for (auto it = std::sregex_iterator(ingredientsList.begin(), ingredientsList.end(), pairRe); it != std::sregex_iterator(); ++it) {
                int qty = std::stoi((*it)[1]);
                if (qty <= 0) { std::cout << "Not enough trophies" << std::endl; return; }
                ingredients[(*it)[2]] += qty;
            }

            inventory.tradeTrophies(trophies);
            for (auto& p : ingredients) inventory.addIngredient(p.first, p.second);
            std::cout << "Trade successful" << std::endl;
            return;
        }

        // Handle ingredient query
        std::regex whatIsInRe(R"(^\s*What\s+is\s+in\s+(.+?)\s*\?$)", std::regex::icase);
        if (std::regex_match(line, match, whatIsInRe)) {
            std::string potion = strip(match[1]);
            std::string normPotion = normalize(potion);
            if (!alchemy.hasFormula(normPotion)) {
                std::cout << "No formula for " << potion << std::endl;
                return;
            }
            auto recipe = alchemy.getFormula(normPotion);
            std::vector<std::pair<std::string, int>> sorted(recipe.begin(), recipe.end());
            std::sort(sorted.begin(), sorted.end());
            for (size_t i = 0; i < sorted.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << sorted[i].second << " " << sorted[i].first;
            }
            std::cout << std::endl;
            return;
        }

        // Handle bestiary query
        std::regex whatEffective(R"(^\s*what\s+is\s+effective\s+against\s+(.+?)\s*\?$)", std::regex::icase);
        if (std::regex_match(line, match, whatEffective)) {
            std::string monster = strip(match[1]);
            if (!bestiary.hasEffectiveAgainst(monster)) {
                std::cout << "No knowledge of " << monster << std::endl;
                return;
            }
            auto items = bestiary.getEffectiveItems(monster);
            if (items.empty()) {
                std::cout << "No knowledge of " << monster << std::endl;
            } else {
                std::vector<std::string> sorted(items.begin(), items.end());
                std::sort(sorted.begin(), sorted.end());
                for (size_t i = 0; i < sorted.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << sorted[i];
                }
                std::cout << std::endl;
            }
            return;
        }

        // Handle total count for specific item
        std::regex totalSingle(R"(.*total\s+(ingredient|potion|trophy)\s+(.+?)\s*\?)", std::regex::icase);
        if (std::regex_match(line, match, totalSingle)) {
            std::string category = match[1];
            std::string item = strip(match[2]);
            if (category == "ingredient") std::cout << inventory.getIngredient(item) << std::endl;
            else if (category == "potion") std::cout << inventory.getPotion(item) << std::endl;
            else if (category == "trophy") std::cout << inventory.getTrophy(item) << std::endl;
            else std::cout << "INVALID" << std::endl;
            return;
        }

        // Handle total count for all items in a category
        std::regex totalAll(R"(.*total\s+(ingredient|potion|trophy)\s*\?)", std::regex::icase);
        if (std::regex_match(line, match, totalAll)) {
            std::string category = match[1];
            auto printSorted = [](const std::map<std::string, int>& data) {
                std::vector<std::pair<std::string, int>> sorted(data.begin(), data.end());
                sorted.erase(std::remove_if(sorted.begin(), sorted.end(), [](auto& p) {
                    return p.second == 0;
                }), sorted.end());
                std::sort(sorted.begin(), sorted.end());
                if (sorted.empty()) { std::cout << "None" << std::endl; return; }
                for (size_t i = 0; i < sorted.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << sorted[i].second << " " << sorted[i].first;
                }
                std::cout << std::endl;
            };
            if (category == "ingredient") printSorted(inventory.getAllIngredients());
            else if (category == "potion") printSorted(inventory.getAllPotions());
            else if (category == "trophy") printSorted(inventory.getAllTrophies());
            else std::cout << "INVALID" << std::endl;
            return;
        }

        std::cout << "INVALID" << std::endl;
    }
};
