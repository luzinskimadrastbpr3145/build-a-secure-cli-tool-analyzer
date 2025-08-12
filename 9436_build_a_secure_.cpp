#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <regex>
#include <openssl/sha.h>
#include <openssl/md5.h>

using namespace std;

map<string, string> configMap;

bool loadConfig(const string& configFile) {
    ifstream file(configFile);
    if (!file) {
        return false;
    }

    string line;
    while (getline(file, line)) {
        size_t pos = line.find("=");
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);
            configMap[key] = value;
        }
    }

    return true;
}

string sha256(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    return ss.str();
}

string md5(const string& input) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, input.c_str(), input.size());
    MD5_Final(hash, &md5);

    stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <config_file> <binary_file>" << endl;
        return 1;
    }

    string configFile = argv[1];
    if (!loadConfig(configFile)) {
        cout << "Failed to load config file" << endl;
        return 1;
    }

    string binaryFile = argv[2];
    ifstream file(binaryFile, ios::binary);
    if (!file) {
        cout << "Failed to open binary file" << endl;
        return 1;
    }

    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    string sha256Hash = sha256(string(buffer.begin(), buffer.end()));
    string md5Hash = md5(string(buffer.begin(), buffer.end()));

    regex regexSha256(configMap["sha256_regex"]);
    regex regexMd5(configMap["md5_regex"]);

    if (regex_match(sha256Hash, regexSha256) && regex_match(md5Hash, regexMd5)) {
        cout << "Binary file is valid" << endl;
        return 0;
    } else {
        cout << "Binary file is not valid" << endl;
        return 1;
    }
}