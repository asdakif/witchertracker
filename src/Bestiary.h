#pragma once
#include <string>
#include <map>
#include <set>

class Bestiary {
private:
    std::map<std::string, std::set<std::string>> effectiveness;

public:
    std::string addEffectiveness(const std::string& monster, const std::string& item) {
        auto& set = effectiveness[monster];
        if (set.count(item)) return "Already known effectiveness";
        bool isNew = set.empty();
        set.insert(item);
        return isNew ? "New bestiary entry added: " + monster : "Bestiary entry updated: " + monster;
    }

    bool hasEffectiveAgainst(const std::string& monster) const {
        return effectiveness.count(monster);
    }

    std::set<std::string> getEffectiveItems(const std::string& monster) const {
        auto it = effectiveness.find(monster);
        return (it != effectiveness.end()) ? it->second : std::set<std::string>{};
    }
};
