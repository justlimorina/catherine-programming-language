#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <sstream>

enum class ValueType {
    NIL,
    NUMBER,
    STRING,
    BOOLEAN,
    ARRAY,
    STRUCT
};

struct Value {
    ValueType type;
    double numberValue;
    std::string stringValue;
    bool boolValue;

    // Array data
    std::vector<Value> arrayValue;

    // Struct data
    std::string structTypeName;
    std::unordered_map<std::string, Value> structMembers;

    Value() : type(ValueType::NIL), numberValue(0), stringValue(""), boolValue(false) {}
    explicit Value(double num) : type(ValueType::NUMBER), numberValue(num), stringValue(""), boolValue(false) {}
    explicit Value(std::string str) : type(ValueType::STRING), numberValue(0), stringValue(std::move(str)), boolValue(false) {}
    explicit Value(bool b) : type(ValueType::BOOLEAN), numberValue(0), stringValue(""), boolValue(b) {}
    explicit Value(std::vector<Value> arr) : type(ValueType::ARRAY), numberValue(0), stringValue(""), boolValue(false), arrayValue(std::move(arr)) {}
    Value(std::string typeName, std::unordered_map<std::string, Value> members)
        : type(ValueType::STRUCT), numberValue(0), stringValue(""), boolValue(false), structTypeName(std::move(typeName)), structMembers(std::move(members)) {}

    std::string toString() const {
        switch (type) {
            case ValueType::NUMBER: {
                std::ostringstream ss;
                ss << numberValue;
                return ss.str();
            }
            case ValueType::STRING:
                return stringValue;
            case ValueType::BOOLEAN:
                return boolValue ? "true" : "false";
            case ValueType::ARRAY: {
                std::string res = "[";
                for (size_t i = 0; i < arrayValue.size(); ++i) {
                    res += arrayValue[i].toString() + (i + 1 < arrayValue.size() ? ", " : "");
                }
                res += "]";
                return res;
            }
            case ValueType::STRUCT: {
                std::string res = structTypeName + " {";
                size_t idx = 0;
                for (const auto& kv : structMembers) {
                    res += kv.first + ": " + kv.second.toString() + (idx + 1 < structMembers.size() ? ", " : "");
                    idx++;
                }
                res += "}";
                return res;
            }
            default:
                return "nil";
        }
    }

    bool isTruthy() const {
        if (type == ValueType::BOOLEAN) return boolValue;
        if (type == ValueType::NUMBER) return numberValue != 0;
        if (type == ValueType::STRING) return !stringValue.empty();
        if (type == ValueType::ARRAY) return !arrayValue.empty();
        if (type == ValueType::STRUCT) return true;
        return false;
    }
};

#endif // VALUE_HPP
