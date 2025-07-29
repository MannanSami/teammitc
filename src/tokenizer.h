#pragma once

enum class TokenType {
    exit,
    int_lit,
    semi,
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

class Tokenizer {
public:
    inline explicit Tokenizer(std::string src): m_src(std::move(src)) {}

    std::vector<Token> tokenize() {
        std::string buffer;
        std::vector<Token> tokens;

        while (peek().has_value()) {
            if (isalpha(peek().value())) {
                buffer.push_back(consume());

                while (peek().has_value() && isalnum(peek().value())) {
                    buffer.push_back(consume());
                }
                if (buffer == "exit") {
                    tokens.push_back({ .type = TokenType::exit });
                    buffer.clear();
                    continue;
                } else {
                    std::cerr << "Unrecognized token type: " << buffer << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else if (isdigit(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && isdigit(peek().value())) {
                    buffer.push_back(consume());
                }
                tokens.push_back({ .type = TokenType::int_lit, .value = buffer });
                buffer.clear();
                continue;
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({ .type = TokenType::semi });
                continue;
            } else if (isspace(peek().value())) {
                consume();
                continue;
            } else {
                std::cerr << "Unrecognized token type: " << buffer << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_current_idx = 0;
        return tokens;
    }
private:
    [[nodiscard]] std::optional<char> peek(const int offset = 1) const {
        if (m_current_idx + offset > m_src.length()) {
            return {};
        }
        return m_src.at(m_current_idx);
    }

    char consume() {
        return m_src.at(m_current_idx++);
    }

    std::string m_src;
    int m_current_idx = 0;
};