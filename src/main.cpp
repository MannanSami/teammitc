#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

enum class TokenType {
    exit,
    int_lit,
    semi,
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string buffer;
    for (int i = 0; i < input.size(); i++) {
        const char c = input.at(i);
        if (isalpha(c) || c == '_') {
            buffer.push_back(c);
            i++;
            while (isalnum(input.at(i))) {
                buffer.push_back(input.at(i));
                i++;
            }
            i--;
            if (buffer == "exit") {
                tokens.push_back({.type = TokenType::exit});
                buffer.clear();
                continue;
            }  else {
                std::cerr << "Unrecognized token type: " << buffer << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else if (isdigit(c)) {
            buffer.push_back(c);
            i++;
            while (isdigit(input.at(i))) {
                buffer.push_back(input.at(i));
                i++;
            }
            i--;
            tokens.push_back({.type = TokenType::int_lit, .value = buffer});
            buffer.clear();
        } else if (isspace(c)) {
            continue;
        } else if (c == ';') {
            tokens.push_back({.type = TokenType::semi});
        } else {
            std::cerr << "Unrecognized token type: " << buffer << std::endl;
        }
    }
    return tokens;
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output;
    output << ".global _main\n_main:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov X0, #" << tokens.at(i + 1).value.value() << "\n";
                    output << "    mov X16, #1\n";
                    output << "    svc 0";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Incorrect Usage. Correct Usage: " << std::endl;
        std::cerr << "teammitc <input.tmt>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents; {
        std::stringstream content_stream;
        std::fstream input(argv[1], std::ios::in);
        if (!input) {
            std::cerr << "Error: Could not open file '" << argv[1] << "'" << std::endl;
            return EXIT_FAILURE;
        }
        content_stream << input.rdbuf();
        contents = content_stream.str();
        if (contents.empty()) {
            std::cerr << "Warning: Input file '" << argv[1] << "' is empty." << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::vector<Token> tokens = tokenize(contents);
    std::println("{}", tokens_to_asm(tokens));
    {
        std::fstream file("output.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("as -o output.o output.asm");
    system("ld -o output output.o -lSystem -syslibroot `xcrun --sdk macosx --show-sdk-path` -e _main -arch arm64");


    return EXIT_SUCCESS;
}