#pragma once

#include <stdio.h>
#include "common/common.h"
#include <direct.h>
//#include "src/generated_config_struct_enum.h"

const std::string indent = "    ";

void AddCommentEnum(AeXMLNode& node, AeFile& file) {
    for (auto& comment : node.data->comments) {
        if (!comment.compare(0, 5, "enum ")) {
            file.addNewLine("");
            std::vector<std::string> ss = ENCODE->split<std::string>(comment, " ");
            std::string s = "enum " + ss[2] + " {";
            file.addNewLine(s.c_str());
            for (size_t i = 3; i < ss.size(); i += 2) {
                s = indent + ss[i] + " = " + ss[i + 1] + ",";
                file.addNewLine(s.c_str());
            }
            file.addNewLine("};");
        }
    }
    for (auto next : node.data->nexts) {
        AddCommentEnum(*next, file);
    }
}

std::string AddElementCode(AeNode& element, const std::string& type, std::vector<std::string>& load_codes) {
    if (element.value.length() != 2) {
        load_codes.push_back(element.key + " = property_->getXMLValue<" + type + ">(\"" + element.key + "\");");
        return (type + " " +element.key+";");
    }
    std::string type1 = "AeVector<" + type + ", " + element.value.at(1) + ">";
    load_codes.push_back(element.key + " = property_->getXMLValues<" + type + ", " + element.value.at(1) + ">(\"" + element.key +
                         "\");");
    return (type1 + " " + element.key + ";");
}

void AddGameObjectComponentStrcut(AeXMLNode& node, AeFile& file) {
    // struct
    file.addNewLine("");
    std::string s = "struct AeGameObjectComponent" + node.data->key + " {";
    file.addNewLine(s.c_str());
    auto* define_node = node.getXMLNode("define");
    std::vector<std::string> load_codes;

    for (auto& element : define_node->data->elements) {
        s = indent;
        if (!element.key.compare("type")) {
            s += "AE_GAMEOBJECT_TYPE type;";
            load_codes.push_back("type = static_cast<AE_GAMEOBJECT_TYPE>(property_->getXMLValue<int>(\"type\"));");
        } else {
            switch (element.value.at(0)) {
                case 'i':
                    s += AddElementCode(element, "int", load_codes);
                    break;
                case 'f':
                    s += AddElementCode(element, "float", load_codes);
                    break;
                case 'b':
                    s += AddElementCode(element, "bool", load_codes);
                    break;
                case 'e':
                    for (auto& comment : define_node->data->comments) {
                        std::vector<std::string> ss = ENCODE->split<std::string>(comment, " ");
                        if (!element.key.compare(ss[1])) {
                            s += (ss[2] + " " + element.key + ";");
                            load_codes.push_back(element.key + " = static_cast<" + ss[2] + ">(property_->getXMLValue<int>(\"" +
                                                 element.key + "\"));");
                            break;
                        }
                    }
                    break;
                case 's':
                    s += ("std::string " + element.key + ";");
                    load_codes.push_back(element.key + " = property_->getXMLValue<std::string>(\"" + element.key + "\");");
                    break;
                default:
                    break;
            }
        }
        file.addNewLine(s.c_str());
    }

    // read xml
    file.addNewLine("");
    file.addNewLine((indent + "AeXMLNode* property_;").c_str());
    file.addNewLine((indent + "void read(AeXMLNode& node) {").c_str());
    file.addNewLine((indent + indent + "property_ = &node;").c_str());
    for (auto& load_code : load_codes) {
        file.addNewLine((indent + indent + load_code).c_str());
    }
    file.addNewLine((indent + "}").c_str());

    file.addNewLine((indent + "void reset() { read(*property_); }").c_str());
    file.addNewLine("};");
}

int main(int argc, char* argv[]) {
    // generated_config_struct.h though config.xml
    LOG("Create src\\generated_config_struct.h");
    AeFile file;
    file.open("..\\..\\src\\generated_config_struct_enum.h");
    file.addNewLine("#pragma once");
    file.addNewLine("//This file is generated by code_generator.cpp.DO NOT edit this file.");
    file.addNewLine("#include \"common/common.h\"");
    // file.addNewLine("#include <cstring>");

    auto* config = AST->getXML("..\\..\\output\\data\\config.xml");

    // enum gameobject
    file.addNewLine("");
    file.addNewLine("enum AE_GAMEOBJECT_TYPE {");
    std::string key = indent + "eGAMEOBJECT_";

    auto* node = config->getXMLNode("scenes.define");
    std::string enum_name = key + "Scene = " + node->getXMLValue<int>("type") + ",";
    file.addNewLine(enum_name.c_str());

    node = config->getXMLNode("objects.define");
    enum_name = key + "Object = " + node->getXMLValue<int>("type") + ",";
    file.addNewLine(enum_name.c_str());

    node = config->getXMLNode("components");
    key += "Component_";
    for (auto comp : node->data->nexts) {
        enum_name = key + comp->data->key + " = " + comp->getXMLValue<int>("define.type") + ",";
        file.addNewLine(enum_name.c_str());
    }
    file.addNewLine("};");
    // comment enum
    AddCommentEnum(*config, file);

    // struct
    node = config->getXMLNode("components");
    for (auto comp : node->data->nexts) {
        AddGameObjectComponentStrcut(*comp, file);
    }

    file.close();
    return 0;
}
