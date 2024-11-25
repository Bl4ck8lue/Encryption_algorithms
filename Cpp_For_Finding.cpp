#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <random>
#include <array>
#include <vector>
#include <locale.h>

using namespace std;

class SHA256 {
public:
    SHA256() {
        reset();
    }

    void update(const std::string& data) {
        for (unsigned char c : data) {
            buffer[bufferSize++] = c;
            if (bufferSize == BlockSize) {
                processBlock(buffer.data());
                totalLength += BlockSize * 8;
                bufferSize = 0;
            }
        }
    }

    std::string final() {
        uint64_t totalBits = totalLength + bufferSize * 8;

        buffer[bufferSize++] = 0x80;
        if (bufferSize > BlockSize - 8) {
            while (bufferSize < BlockSize) {
                buffer[bufferSize++] = 0x00;
            }
            processBlock(buffer.data());
            bufferSize = 0;
        }

        while (bufferSize < BlockSize - 8) {
            buffer[bufferSize++] = 0x00;
        }

        for (int i = 7; i >= 0; --i) {
            buffer[BlockSize - 8 + i] = totalBits & 0xFF;
            totalBits >>= 8;
        }

        processBlock(buffer.data());

        std::ostringstream result;
        for (const auto& word : hash) {
            result << std::hex << std::setw(8) << std::setfill('0') << word;
        }

        reset();
        return result.str();
    }

private:
    static constexpr size_t BlockSize = 64;
    static constexpr size_t HashSize = 8;

    std::array<uint32_t, HashSize> hash{};
    std::vector<uint8_t> buffer = std::vector<uint8_t>(BlockSize);
    size_t bufferSize = 0;
    uint64_t totalLength = 0;

    static constexpr std::array<uint32_t, 64> k = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    static uint32_t rightRotate(uint32_t x, uint32_t n) {
        return (x >> n) | (x << (32 - n));
    }

    void reset() {
        hash = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };
        bufferSize = 0;
        totalLength = 0;
    }

    void processBlock(const uint8_t* block) {
        std::array<uint32_t, 64> w;
        for (size_t i = 0; i < 16; ++i) {
            w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | block[i * 4 + 3];
        }

        for (size_t i = 16; i < 64; ++i) {
            uint32_t s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        std::array<uint32_t, 8> workingVars = hash;

        for (size_t i = 0; i < 64; ++i) {
            uint32_t S1 = rightRotate(workingVars[4], 6) ^ rightRotate(workingVars[4], 11) ^ rightRotate(workingVars[4], 25);
            uint32_t ch = (workingVars[4] & workingVars[5]) ^ (~workingVars[4] & workingVars[6]);
            uint32_t temp1 = workingVars[7] + S1 + ch + k[i] + w[i];
            uint32_t S0 = rightRotate(workingVars[0], 2) ^ rightRotate(workingVars[0], 13) ^ rightRotate(workingVars[0], 22);
            uint32_t maj = (workingVars[0] & workingVars[1]) ^ (workingVars[0] & workingVars[2]) ^ (workingVars[1] & workingVars[2]);
            uint32_t temp2 = S0 + maj;
            workingVars[7] = workingVars[6];
            workingVars[6] = workingVars[5];
            workingVars[5] = workingVars[4];
            workingVars[4] = workingVars[3] + temp1;
            workingVars[3] = workingVars[2];
            workingVars[2] = workingVars[1];
            workingVars[1] = workingVars[0];
            workingVars[0] = temp1 + temp2;
        }

        for (size_t i = 0; i < HashSize; ++i) {
            hash[i] += workingVars[i];
        }
    }
};

string generateRandomString(size_t length = 10) {
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t max_index = charset.size() - 1;
    string randomString;
    static random_device rd;
    static mt19937 generator(rd());
    static uniform_int_distribution<> distribution(0, max_index);

    for (size_t i = 0; i < length; ++i) {
        randomString += charset[distribution(generator)];
    }
    return randomString;
}

int main() {
    setlocale(LC_ALL, "Russian");

    unordered_map<string, string> hashMap;

    // Ввод текста от пользователя
    string input;
    cout << "Введите текст для хэширования: ";
    getline(cin, input);

    // Создание хэша введенного текста
    SHA256 sha256;
    sha256.update(input);
    string inputHash = sha256.final();

    cout << "SHA-256 хэш введенного текста: " << inputHash << endl;

    size_t attempts = 0;
    const size_t MAX_ATTEMPTS = 1000000; // Установите лимит
    bool check = false;

    while (attempts < MAX_ATTEMPTS) {
        ++attempts;
        string randomString = generateRandomString();

        // Создание хэша случайной строки
        sha256.update(randomString);
        string randomHash = sha256.final();

        if (hashMap.find(randomHash) != hashMap.end()) {
            cout << "Коллизия найдена после " << attempts << " попыток!" << endl;
            cout << "Строка из хэш-карты: " << hashMap[randomHash] << endl;
            cout << "Случайная строка: " << randomString << endl;
            cout << "Хэш: " << randomHash << endl;
            check = true;
            break;
        }

        hashMap[randomHash] = randomString; // Сохраняем хэш и строку
    }

    if (check != true) {
        cout << "Коллизии не найдены :-)\n";
    }

    return 0;
}
