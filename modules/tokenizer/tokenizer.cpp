#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <jsoncpp/json/json.h>

class Tokenizer {
public:
    std::vector<std::string> tokenize(const std::string& text) {
        std::vector<std::string> tokens;
        std::string token;
        
        for (size_t i = 0; i < text.length(); ++i) {
            char c = text[i];
            if (std::isspace(c) || std::ispunct(c)) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

int main() {
    std::string line;
    std::getline(std::cin, line);
    
    Tokenizer t;
    auto tokens = t.tokenize(line);
    
    Json::Value root;
    for (const auto& token : tokens) {
        root["tokens"].append(token);
    }
    root["count"] = (int)tokens.size();
    root["status"] = "success";
    
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  ";
    std::cout << Json::writeString(writer, root);
    
    return 0;
}
