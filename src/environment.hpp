#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "value.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

class Environment : public std::enable_shared_from_this<Environment> {
public:
    explicit Environment(std::shared_ptr<Environment> parentEnv = nullptr)
        : parent(std::move(parentEnv)) {}

    void define(const std::string& name, const Value& val) {
        values[name] = val;
    }

    void assign(const std::string& name, const Value& val) {
        if (values.find(name) != values.end()) {
            values[name] = val;
            return;
        }
        if (parent) {
            parent->assign(name, val);
            return;
        }
        // If not found in any scope, define in current scope
        values[name] = val;
    }

    Value get(const std::string& name) const {
        auto it = values.find(name);
        if (it != values.end()) {
            return it->second;
        }
        if (parent) {
            return parent->get(name);
        }
        return Value(); // Return nil if uninitialized
    }

    bool contains(const std::string& name) const {
        if (values.find(name) != values.end()) return true;
        if (parent) return parent->contains(name);
        return false;
    }

    std::shared_ptr<Environment> getParent() const {
        return parent;
    }

private:
    std::shared_ptr<Environment> parent;
    std::unordered_map<std::string, Value> values;
};

#endif // ENVIRONMENT_HPP
