#ifndef CRC_H_
#define CRC_H_
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <bitset>

using namespace std;

inline string fromDecimalToBinaryString(long long int decimalNumber)
{
    string binaryString = "";
    while (decimalNumber)
    {
        if (decimalNumber & 1)
            binaryString = "1" + binaryString;
        else
            binaryString = "0" + binaryString;
        decimalNumber = decimalNumber >> 1;
    }
    return binaryString;
}

inline long long int fromBinaryStringToDecimal(string binaryString)
{
    long long int decimalNumber = 0;
    for (int i = 0; i < binaryString.size(); i++)
        if (binaryString[i] == '1')
            decimalNumber += 1 << (binaryString.length() - i - 1);
    return decimalNumber;
}

string getBinaryStringFromPayload(string payload)
{
    string binaryString = ""; // initial value to append to
    // loop through each char
    for (int i = 0; i < payload.size(); i++)
    {
        bitset<8> charBits(payload[i]);
        binaryString += charBits.to_string();
    }
    return binaryString;
}

inline long long int remainderOfBinaryLongdivison(long long int dividend, long long gen, int l_gen)
{
    // shft specifies the no. of least
    // significant bits not being XORed
    int shft = (int)ceill(log2l(dividend + 1)) - l_gen;
    long long int rem;

    while ((dividend >= gen) || (shft >= 0))
    {
        // bitwise XOR the MSBs of dividend with generator
        // replace the operated MSBs from the dividend with
        // remainder generated
        rem = (dividend >> shft) ^ gen;
        dividend = (dividend & ((1 << shft) - 1)) | (rem << shft);

        // change shft variable
        shft = (int)ceill(log2l(dividend + 1)) - l_gen;
    }
    return dividend;
}

// returns the remainder of the CRC
inline long long int getRemainderCRC(string payload)
{
    string generatorBinaryString = "1001";
    int l_gen = generatorBinaryString.length();
    long long int gen = fromBinaryStringToDecimal(generatorBinaryString);
    string payloadBinaryString = getBinaryStringFromPayload(payload);
    long long int dword = fromBinaryStringToDecimal(payloadBinaryString);
    // append 0s to dividend by shifting
    long long int dividend = dword << (l_gen - 1);
    return remainderOfBinaryLongdivison(dividend, gen, l_gen);
}

// returns true if the check is valid and false if there is an error
inline bool validCRC(string possiblyModifiedPayload, long long int remainder)
{
    string generatorBinaryString = "1001";
    int l_gen = generatorBinaryString.length();
    long long int gen = fromBinaryStringToDecimal(generatorBinaryString);
    long long int dword = fromBinaryStringToDecimal(getBinaryStringFromPayload(possiblyModifiedPayload));
    // shifting the message
    long long int dividend = dword << (l_gen - 1);
    // appending the remainder
    dividend += remainder;
    return remainderOfBinaryLongdivison(dividend, gen, l_gen) == 0;
}
#endif
