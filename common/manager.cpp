#include "common.h"
#include <sstream>
#include <stdarg.h>

SINGLETON_INSTANCE(AeCommonManager)

std::map<std::string, AeXMLNode *> astXMLs;
std::map<std::string, AeJSONNode *> astJSONs;

AeXMLNode::AeXMLNode() : data(new AeXMLData()) {}

AeXMLNode::~AeXMLNode() {
    std::vector<AeXMLNode *>::iterator it = data->nexts.begin();
    while (it != data->nexts.end()) {
        if ((*it) != nullptr) delete (*it);
        ++it;
    }
    data->nexts.clear();
    delete data;
    data = nullptr;
}

AeXMLNode *AeXMLNode::getXMLNode(const char *key) { return getXMLNode(COM_ENCODE.split<std::string>(key, ".")); }

AeXMLNode *AeXMLNode::getXMLNode(std::vector<std::string> &keys) {
    AeXMLNode *current = this;
    for (const auto &key : keys) {
        bool b = true;
        for (const auto &node : current->data->nexts) {
            if (key.compare(node->data->key) == 0) {
                current = node;
                b = false;
                break;
            }
        }
        if (b) return nullptr;
    }
    return current;
}

AeXMLNode *AeXMLNode::copyXMLNode() {
    AeXMLNode *node = new AeXMLNode();
    copyXMLNode(node);
    return node;
}

void AeXMLNode::copyXMLNode(AeXMLNode *to) {
    copyXMLValue(to);

    for (const auto &node : to->data->nexts) {
        if (node) delete node;
    }
    to->data->nexts.clear();

    for (const auto &node : data->nexts) {
        AeXMLNode *new_node = new AeXMLNode();
        to->data->nexts.push_back(new_node);
        node->copyXMLNode(new_node);
    }
}

void AeXMLNode::copyXMLValue(AeXMLNode *to) {
    to->data->comments = data->comments;
    to->data->key = data->key;
    to->data->value = data->value;
    to->data->elements = data->elements;
}

void AeXMLNode::addXMLNode(AeXMLNode *node) { data->nexts.push_back(node); }

void AeXMLNode::setXMLKey(const char *key) { this->data->key = key; }
void AeXMLNode::setXMLValue(const char *value) { this->data->value = value; }

void AeXMLNode::setXMLValue(const char *key, const char *value) {
    for (auto &node : data->elements) {
        if (node.key.compare(key) == 0) {
            node.value = value;
            return;
        }
    }
    AeNode node = {key, value};
    data->elements.push_back(node);
}

void AeXMLNode::removeXMLNode(AeXMLNode *node) {
    for (int i = 0; i < data->nexts.size(); ++i) {
        if (data->nexts[i] == node) {
            data->nexts.erase(data->nexts.begin() + i);
            delete node;
            return;
        }
    }
    for (const auto &node1 : data->nexts) {
        node1->removeXMLNode(node);
    }
}

void AeXMLNode::outputXML(const char *path, int level, std::string *content) {
    std::string s;

    if (!content) {
        content = &s;
    }
    std::string space = "";
    for (int i = 0; i < level; ++i) {
        space += "    ";
    }
    if (data->version.length()) {
        *content += "<?";
        *content += data->version;
        *content += "?>\n";
    }
    for (const auto &s : data->comments) {
        *content += space;
        *content += "<!--";
        *content += s;
        *content += "-->\n";
    }

    *content += space;
    *content += "<";
    *content += data->key;
    for (const auto &node : data->elements) {
        *content += " ";
        *content += node.key;
        *content += "=\"";
        *content += node.value;
        *content += "\"";
    }
    if (!data->nexts.size()) {
        if (data->value.length()) {
            *content += ">";
            *content += data->value;
            *content += "</";
            *content += data->key;
            *content += ">\n";
        } else {
            *content += " />\n";
        }
    } else {
        *content += ">\n";
        for (const auto &node : data->nexts) {
            node->outputXML(nullptr, level + 1, content);
        }

        *content += space;
        *content += "</";
        *content += data->key;
        *content += ">\n";
    }
    if (path) {
        std::ofstream ofile;
        ofile.open(path);

        ofile << s << std::endl;
        ofile.close();
    }
}

AeJSONNode::AeJSONNode() : data(new AeJSON) {}

AeJSONNode::~AeJSONNode() {
    std::vector<AeJSONNode *>::iterator it = data->eNodes.begin();
    while (it != data->eNodes.end()) {
        if ((*it) != nullptr) delete (*it);
        ++it;
    }
    data->eNodes.clear();

    std::vector<std::vector<AeJSONNode *>>::iterator it1 = data->eArrayNodes.begin();
    while (it1 != data->eArrayNodes.end()) {
        it = it1->begin();
        while (it != it1->end()) {
            if ((*it) != nullptr) delete (*it);
            ++it;
        }
        ++it1;
    }
    data->eArrayNodes.clear();
    delete data;
    data = nullptr;
}

const char *AeJSONNode::getJSONValue(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length);
    va_end(keys);
    delete[] keys1;
    return ret;
}

const char *AeJSONNode::getJSONValue(const char *keys[], int length) {
    AeJSONNode *source = this;

    for (int index = 0; index < length; ++index) {
        if (index == (length - 1)) {
            int size = int(source->data->eKeysforValues.size());
            for (int index1 = 0; index1 < size; ++index1) {
                if (strcmp(keys[index], source->data->eKeysforValues[index1].c_str()) == 0)
                    return source->data->eValues[index1].c_str();
            }
            break;
        }

        int size = int(source->data->eKeysforNodes.size());
        int index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforNodes[index1].c_str()) == 0) {
                source = source->data->eNodes[index1];
                break;
            }
        }
        if (index1 != size) continue;

        size = int(source->data->eKeysforArrayNodes.size());
        index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforArrayNodes[index1].c_str()) == 0) {
                int size2 = int(source->data->eArrayNodes[index1].size());
                int index2 = 0;
                for (; index2 < size2; ++index2) {
                    const char *ret = source->data->eArrayNodes[index1][index2]->getJSONValue(&keys[index + 1], length - index - 1);
                    if (ret != nullptr) return ret;
                }
                break;
            }
        }
        break;
    }
    return nullptr;
}

AeJSONNode *AeJSONNode::getJSONNode(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    AeJSONNode *source = getJSONNode(keys1, length);
    va_end(keys);
    delete[] keys1;
    return source;
}

AeJSONNode *AeJSONNode::getJSONNode(const char *keys[], int length) {
    int size = int(data->eKeysforNodes.size());
    for (int index = 0; index < size; ++index) {
        if (strcmp(keys[0], data->eKeysforNodes[index].c_str()) == 0) {
            if (length == 1) return data->eNodes[index];

            return data->eNodes[index]->getJSONNode(&keys[1], length - 1);
        }
    }

    size = int(data->eKeysforArrayNodes.size());
    for (int index = 0; index < size; ++index) {
        if (strcmp(keys[0], data->eKeysforArrayNodes[index].c_str()) == 0) {
            if (length == 1) return data->eArrayNodes[index][0];

            int size1 = int(data->eArrayNodes[index].size());

            for (int index1 = 0; index1 < size1; ++index1) {
                AeJSONNode *ret = data->eArrayNodes[index][index1]->getJSONNode(&keys[1], length - 1);
                if (ret != nullptr) return ret;
            }
            break;
        }
    }
    return nullptr;
}

std::vector<std::string> *AeJSONNode::getJSONArrayValues(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    std::vector<std::string> *ret = getJSONArrayValues(keys1, length);
    va_end(keys);
    delete[] keys1;
    return ret;
}

std::vector<std::string> *AeJSONNode::getJSONArrayValues(const char *keys[], int length) {
    AeJSONNode *source = this;

    for (int index = 0; index < length; ++index) {
        if (index == (length - 1)) {
            int size = int(source->data->eKeysforArrayValues.size());
            for (int index1 = 0; index1 < size; ++index1) {
                if (strcmp(keys[index], source->data->eKeysforArrayValues[index1].c_str()) == 0)
                    return &source->data->eArrayValues[index1];
            }
            break;
        }

        int size = int(source->data->eKeysforNodes.size());
        int index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforNodes[index1].c_str()) == 0) {
                source = source->data->eNodes[index1];
                break;
            }
        }
        if (index1 != size) continue;

        size = int(source->data->eKeysforArrayNodes.size());
        index1 = 0;
        for (; index1 < size; ++index1) {
            if (strcmp(keys[index], source->data->eKeysforArrayNodes[index1].c_str()) == 0) {
                int size2 = int(source->data->eArrayNodes[index1].size());
                int index2 = 0;
                for (; index2 < size2; ++index2) {
                    std::vector<std::string> *ret =
                        source->data->eArrayNodes[index1][index2]->getJSONArrayValues(&keys[index + 1], length - index - 1);
                    if (ret != nullptr) return ret;
                }
                break;
            }
        }
        break;
    }
    return nullptr;
}

std::vector<AeJSONNode *> *AeJSONNode::getJSONArrayNodes(int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    std::vector<AeJSONNode *> *ret = getJSONArrayNodes(keys1, length);
    va_end(keys);
    delete[] keys1;
    return ret;
}

std::vector<AeJSONNode *> *AeJSONNode::getJSONArrayNodes(const char *keys[], int length) {
    int size = int(data->eKeysforArrayNodes.size());
    for (int index = 0; index < size; ++index) {
        if (strcmp(keys[0], data->eKeysforArrayNodes[index].c_str()) == 0) {
            if (length == 1) return &data->eArrayNodes[index];

            int size1 = int(data->eArrayNodes[index].size());

            for (int index1 = 0; index1 < size1; ++index1) {
                std::vector<AeJSONNode *> *ret = data->eArrayNodes[index][index1]->getJSONArrayNodes(&keys[1], length - 1);
                if (ret != nullptr) return ret;
            }
            break;
        }
    }

    if (length > 1) {
        size = int(data->eKeysforNodes.size());
        for (int index = 0; index < size; ++index) {
            if (strcmp(keys[0], data->eKeysforNodes[index].c_str()) == 0) {
                return data->eNodes[index]->getJSONArrayNodes(&keys[1], length - 1);
            }
        }
    }
    return nullptr;
}

bool AeJSONNode::getJSONbValue(bool *output, int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length + 1);
    va_end(keys);
    delete[] keys1;

    if (ret) *output = atoi(ret);

    return ret;
}

bool AeJSONNode::getJSONiValue(int *output, int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length + 1);
    va_end(keys);
    delete[] keys1;

    if (ret) *output = atoi(ret);

    return ret;
}

bool AeJSONNode::getJSONfValue(float *output, int length, ...) {
    va_list keys;
    va_start(keys, length);

    const char **keys1 = new const char *[length];
    for (int i = 0; i < length; ++i) keys1[i] = va_arg(keys, const char *);

    const char *ret = getJSONValue(keys1, length + 1);
    va_end(keys);
    delete[] keys1;

    if (ret) *output = float(atof(ret));

    return ret;
}

AeCommonManager::AeCommonManager() {}
AeCommonManager::~AeCommonManager() {
    std::map<std::string, AeXMLNode *>::iterator it = astXMLs.begin();
    while (it != astXMLs.end()) {
        if ((it->second) != nullptr) delete (it->second);
        ++it;
    }
    astXMLs.clear();

    std::map<std::string, AeJSONNode *>::iterator it1 = astJSONs.begin();
    while (it1 != astJSONs.end()) {
        if ((it1->second) != nullptr) delete (it1->second);
        ++it1;
    }
    astJSONs.clear();
}

void AeCommonManager::removeXML(std::string path) {
    std::map<std::string, AeXMLNode *>::iterator it = astXMLs.find(path);

    if (it != astXMLs.end()) {
        delete it->second;
        astXMLs.erase(it);
    }
}

std::vector<char> AeCommonManager::loadFile(const char *_filePath) {
    std::vector<char> ret;
    std::ifstream file(_filePath, std::ios::ate | std::ios::binary);
    ASSERT(file.is_open(), _filePath);
    // if (!file.is_open()) return ret;

    file.seekg(0, file.end);
    int length = int(file.tellg());
    file.seekg(0);

    ret.resize(length);
    file.read(ret.data(), length);
    file.close();
    return ret;
}

AeJSONNode *AeCommonManager::getJSON(const char *_filePath) {
    std::map<std::string, AeJSONNode *>::iterator it = astJSONs.find(_filePath);

    if (it != astJSONs.end()) return it->second;

    std::vector<char> buffer = loadFile(_filePath);

    int index = 0;
    AeJSONNode *head = COM_ENCODE.decodeJSON(buffer.data(), index);
    astJSONs[_filePath] = head;
    return head;
}

AeXMLNode *AeCommonManager::getXML(const char *_filePath) {
    std::map<std::string, AeXMLNode *>::iterator it = astXMLs.find(_filePath);

    if (it != astXMLs.end()) return it->second;

    // std::ifstream file(_filePath, std::ios::ate | std::ios::binary);
    // if (!file.is_open()) return nullptr;

    std::vector<char> buffer = loadFile(_filePath);

    int index = 0;
    AeXMLNode *head = COM_ENCODE.decodeXML(buffer.data(), index);
    astXMLs[_filePath] = head;

    return head;
}

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(common)

ID get_serial_number() {
    static ID serial_number = 0;
    ++serial_number;
    return serial_number;
}

IObject::IObject() : id_(get_serial_number()), active_(false) {}

ID IObject::get_id() { return id_; }
bool IObject::active() { return active_; }

AeResult IObject::initialize() {
    active_ = true;
    return AE_SUCCESS;
}
AeResult IObject::cleanup() {
    active_ = false;
    return AE_SUCCESS;
}

END_NAMESPACE(common)
END_NAMESPACE(ae)
