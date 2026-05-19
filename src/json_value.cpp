#include "json_value.h"

#include <charconv>
#include <cctype>
#include <sstream>

namespace text2mdx {

namespace {

class Parser {
public:
    explicit Parser(const std::string& source) : source_(source) {}

    JsonValue parse() {
        skipWhitespace();
        JsonValue value = parseValue();
        skipWhitespace();
        if (!eof()) {
            fail("Unexpected trailing data.");
        }
        return value;
    }

private:
    const std::string& source_;
    std::size_t index_ = 0;

    bool eof() const { return index_ >= source_.size(); }

    char peek() const {
        return eof() ? '\0' : source_[index_];
    }

    char get() {
        if (eof()) {
            fail("Unexpected end of JSON.");
        }
        return source_[index_++];
    }

    void skipWhitespace() {
        while (!eof() && std::isspace(static_cast<unsigned char>(source_[index_]))) {
            ++index_;
        }
    }

    [[noreturn]] void fail(const std::string& message) const {
        std::ostringstream stream;
        stream << "JSON parse error at byte " << index_ << ": " << message;
        throw std::runtime_error(stream.str());
    }

    bool consume(char c) {
        if (peek() == c) {
            ++index_;
            return true;
        }
        return false;
    }

    void expect(char c) {
        if (!consume(c)) {
            std::string message = "Expected '";
            message.push_back(c);
            message += "'.";
            fail(message);
        }
    }

    JsonValue parseValue() {
        skipWhitespace();
        switch (peek()) {
        case 'n':
            parseLiteral("null");
            return JsonValue(nullptr);
        case 't':
            parseLiteral("true");
            return JsonValue(true);
        case 'f':
            parseLiteral("false");
            return JsonValue(false);
        case '"':
            return JsonValue(parseString());
        case '[':
            return JsonValue(parseArray());
        case '{':
            return JsonValue(parseObject());
        default:
            if (peek() == '-' || std::isdigit(static_cast<unsigned char>(peek()))) {
                return JsonValue(parseNumber());
            }
            fail("Expected a JSON value.");
        }
    }

    void parseLiteral(const char* literal) {
        while (*literal) {
            if (get() != *literal++) {
                fail("Invalid literal.");
            }
        }
    }

    std::string parseString() {
        expect('"');
        std::string result;

        while (true) {
            const char c = get();
            if (c == '"') {
                break;
            }
            if (c == '\\') {
                const char escaped = get();
                switch (escaped) {
                case '"':
                case '\\':
                case '/':
                    result.push_back(escaped);
                    break;
                case 'b':
                    result.push_back('\b');
                    break;
                case 'f':
                    result.push_back('\f');
                    break;
                case 'n':
                    result.push_back('\n');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                case 'u':
                    appendUtf8Codepoint(result, parseHexCodepoint());
                    break;
                default:
                    fail("Invalid escape sequence.");
                }
            } else {
                result.push_back(c);
            }
        }

        return result;
    }

    int parseHexCodepoint() {
        int codepoint = 0;
        for (int i = 0; i < 4; ++i) {
            const char c = get();
            codepoint <<= 4;
            if (c >= '0' && c <= '9') {
                codepoint += c - '0';
            } else if (c >= 'a' && c <= 'f') {
                codepoint += 10 + c - 'a';
            } else if (c >= 'A' && c <= 'F') {
                codepoint += 10 + c - 'A';
            } else {
                fail("Invalid unicode escape.");
            }
        }
        return codepoint;
    }

    void appendUtf8Codepoint(std::string& result, int codepoint) {
        if (codepoint <= 0x7F) {
            result.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else {
            result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
    }

    double parseNumber() {
        const std::size_t start = index_;
        if (consume('-')) {}
        if (consume('0')) {
        } else {
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                fail("Invalid number.");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++index_;
            }
        }

        if (consume('.')) {
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                fail("Invalid fraction.");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++index_;
            }
        }

        if (peek() == 'e' || peek() == 'E') {
            ++index_;
            if (peek() == '+' || peek() == '-') {
                ++index_;
            }
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                fail("Invalid exponent.");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++index_;
            }
        }

        return std::stod(source_.substr(start, index_ - start));
    }

    JsonValue::Array parseArray() {
        expect('[');
        skipWhitespace();
        JsonValue::Array array;
        if (consume(']')) {
            return array;
        }

        while (true) {
            array.push_back(parseValue());
            skipWhitespace();
            if (consume(']')) {
                break;
            }
            expect(',');
        }
        return array;
    }

    JsonValue::Object parseObject() {
        expect('{');
        skipWhitespace();
        JsonValue::Object object;
        if (consume('}')) {
            return object;
        }

        while (true) {
            skipWhitespace();
            if (peek() != '"') {
                fail("Expected object key.");
            }
            std::string key = parseString();
            skipWhitespace();
            expect(':');
            object.emplace(std::move(key), parseValue());
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            expect(',');
        }
        return object;
    }

};

const std::string emptyString;

} // namespace

bool JsonValue::asBool(bool fallback) const {
    if (const auto* value = std::get_if<bool>(&value_)) {
        return *value;
    }
    return fallback;
}

double JsonValue::asNumber(double fallback) const {
    if (const auto* value = std::get_if<double>(&value_)) {
        return *value;
    }
    return fallback;
}

const std::string& JsonValue::asString() const {
    if (const auto* value = std::get_if<std::string>(&value_)) {
        return *value;
    }
    throw std::runtime_error("JSON value is not a string.");
}

const std::string& JsonValue::asString(const std::string& fallback) const {
    if (const auto* value = std::get_if<std::string>(&value_)) {
        return *value;
    }
    return fallback;
}

const JsonValue::Array& JsonValue::asArray() const {
    if (const auto* value = std::get_if<Array>(&value_)) {
        return *value;
    }
    throw std::runtime_error("JSON value is not an array.");
}

const JsonValue::Object& JsonValue::asObject() const {
    if (const auto* value = std::get_if<Object>(&value_)) {
        return *value;
    }
    throw std::runtime_error("JSON value is not an object.");
}

const JsonValue* JsonValue::find(const std::string& key) const {
    if (const auto* object = std::get_if<Object>(&value_)) {
        const auto it = object->find(key);
        if (it != object->end()) {
            return &it->second;
        }
    }
    return nullptr;
}

const JsonValue& JsonValue::at(const std::string& key) const {
    const JsonValue* value = find(key);
    if (!value) {
        throw std::runtime_error("Missing JSON key: " + key);
    }
    return *value;
}

JsonValue parseJson(const std::string& source) {
    return Parser(source).parse();
}

} // namespace text2mdx
