#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <jsoncpp/json/json.h>

class NER {
public:
    struct Entity {
        std::string text;
        std::string type;
        int start;
        int end;
    };
    
    std::vector<Entity> extract_entities(const std::string& text) {
        std::vector<Entity> entities;
        std::string current_entity;
        int start_pos = 0;
        
        for (size_t i = 0; i < text.length(); ++i) {
            char c = text[i];
            
            if (std::isupper(c)) {
                if (current_entity.empty()) {
                    start_pos = i;
                }
                current_entity += c;
            } else if (std::islower(c)) {
                current_entity += c;
            } else {
                if (!current_entity.empty() && current_entity.length() > 1) {
                    entities.push_back({
                        current_entity,
                        "ORG",
                        start_pos,
                        (int)i
                    });
                }
                current_entity.clear();
            }
        }
        
        if (!current_entity.empty() && current_entity.length() > 1) {
            entities.push_back({
                current_entity,
                "ORG",
                start_pos,
                (int)text.length()
            });
        }
        
        return entities;
    }
};

int main() {
    std::string line;
    std::getline(std::cin, line);
    
    NER ner;
    auto entities = ner.extract_entities(line);
    
    Json::Value root;
    for (const auto& entity : entities) {
        Json::Value e;
        e["text"] = entity.text;
        e["type"] = entity.type;
        e["start"] = entity.start;
        e["end"] = entity.end;
        root["entities"].append(e);
    }
    root["count"] = (int)entities.size();
    root["status"] = "success";
    
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  ";
    std::cout << Json::writeString(writer, root);
    
    return 0;
}
