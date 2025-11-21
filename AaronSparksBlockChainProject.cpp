// Aaron Sparks Block Chain Project
#include <NTL/ZZ.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>
using namespace std;
using namespace NTL;

// Helper functions, some reused from SecureRemotePassword project
// Turns a hex string into bytes
string hex_to_bytes(const string &hex)
{
    string out;
    out.reserve(hex.size() / 2);

    for (size_t i = 0; i < hex.size(); i += 2)
    {
        unsigned int byte = 0;
        string byte_str = hex.substr(i, 2);
        stringstream ss;
        ss << std::hex << byte_str;
        ss >> byte;
        out.push_back(static_cast<unsigned char>(byte));
    }
    return out;
}
// OpenSSL SHA256 Hashing Function, helper for taking in strings
string SHA256_string(const string &data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(
        reinterpret_cast<const unsigned char *>(data.data()),
        data.size(),
        hash);
    return string(reinterpret_cast<char *>(hash), SHA256_DIGEST_LENGTH);
}
// Converts bytes to hex, used at the end for M1 and M2
string bytes_to_hex(const string &s)
{
    static const char *hex = "0123456789abcdef";
    string out;
    out.reserve(s.size() * 2);
    for (unsigned char c : s)
    {
        out.push_back(hex[c >> 4]);
        out.push_back(hex[c & 0x0F]);
    }
    return out;
}

// ZZ Big Int to Byte Array
void ZZ_to_byte_array(const ZZ &z, unsigned char array[8])
{
    ZZ test = z;
    for (int i = 7; i >= 0; i--)
    {
        array[i] = static_cast<unsigned char>(conv<long>(test & 0xFF));
        test >>= 8;
    }
}

// Small helper struct
struct BlockStruc
{
    ZZ nonce;
    string hash_hex;
};

// Main blockchain calc function
BlockStruc block_calc(const string &hash, const string &quote)
{
    // Difficulty is 24 bits, or 6 digits of zeros
    string prefix = "000000";

    // Convert hash to bytes
    string prev_bytes = hex_to_bytes(hash);
    string quote_bytes = quote;

    while (true)
    {
        // Generates 64-bit nonce
        ZZ nonce = RandomBits_ZZ(64);

        // Converts nonce to byte array
        unsigned char nonce_arr[8];
        ZZ_to_byte_array(nonce, nonce_arr);

        string nonce_bytes(reinterpret_cast<char *>(nonce_arr), 8);

        // Previous hash + nonce + quote bytes
        string data = prev_bytes + nonce_bytes + quote_bytes;

        // SHA hashing of combined data
        string hash_raw = SHA256_string(data);
        string hash_hex = bytes_to_hex(hash_raw);

        // Check if combined data starts with six zeros
        if (hash_hex.rfind(prefix, 0) == 0)
        {
            return {nonce, hash_hex};
        }
    }
}

// Main Function
int main()
{
    string previous_hash = "";
    string quote = "";

    cout << "Previous hash: ";
    getline(cin, previous_hash);

    cout << "Quote:\n> ";
    getline(cin, quote);

    cout << endl
         << "Running!" << endl;

    BlockStruc blocky = block_calc(previous_hash, quote);

    cout << "Nonce: " << blocky.nonce << "\n";
    cout << "Hash: " << blocky.hash_hex << "\n";
}
