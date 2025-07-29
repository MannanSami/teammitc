#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "tokenization.h"

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

    Tokenizer tokenizer(std::move(contents));

    std::vector<Token> tokens = tokenizer.tokenize();
    std::println("{}", tokens_to_asm(tokens));
    {
        std::fstream file("output.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("as -o output.o output.asm");
    system("ld -o output output.o -lSystem -syslibroot `xcrun --sdk macosx --show-sdk-path` -e _main -arch arm64");

    return EXIT_SUCCESS;
}