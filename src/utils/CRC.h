#ifndef CRC_H_
#define CRC_H_
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <bitset>

using namespace std;
inline string getBinaryStringFromPayload(string payload)
{
    string binaryString = "";
    for (int i = 0; i < payload.size(); i++)
    {
        bitset<8> charBits(payload[i]);
        binaryString += charBits.to_string();
    }
    return binaryString;
}

inline string xorTwoBinaryStrings(string s1, string s2)
{
    string xored = "";
    for (int i = 1; i < s2.size(); i++)
        xored += s1[i] == s2[i] ? "0" : "1";
    return xored;
}
inline string longBinaryDivision(string divident, string divisor)
{
    int NumBitsToXOR = divisor.length();
    string dividentXORedPart = divident.substr(0, NumBitsToXOR);

    while (NumBitsToXOR < divident.size())
    {
        dividentXORedPart = dividentXORedPart[0] == '1' ? xorTwoBinaryStrings(divisor, dividentXORedPart) + divident[NumBitsToXOR] : xorTwoBinaryStrings(string(NumBitsToXOR, '0'), dividentXORedPart) + divident[NumBitsToXOR];
        NumBitsToXOR++;
    }
    dividentXORedPart = dividentXORedPart[0] == '1' ? xorTwoBinaryStrings(divisor, dividentXORedPart) : xorTwoBinaryStrings(string(NumBitsToXOR, '0'), dividentXORedPart);
    return dividentXORedPart;
}

inline string getRemainderCRC(string payload)
{
    string generator = "1001";
    string binaryStringPayload = getBinaryStringFromPayload(payload);
    string shiftedPayload = (binaryStringPayload + string(generator.size() - 1, '0'));
    string remainder = longBinaryDivision(shiftedPayload, generator);
    return remainder;
}

inline bool validCRC(string payload, string remainder)
{
    string generator = "1001";
    string remainderBinaryString = remainder;
    string binaryStringPayload = getBinaryStringFromPayload(payload);
    while (remainderBinaryString.size() < generator.size() - 1)
        remainderBinaryString = "0" + remainderBinaryString;
    string messageAddedWithRemainder = binaryStringPayload + remainderBinaryString;
    string returnedRemainder = longBinaryDivision(messageAddedWithRemainder, generator);
    cout << returnedRemainder << endl;
    for (int i = 0; i < returnedRemainder.size(); i++)
        if (returnedRemainder[i] == '1')
            return false;
    return true;
}
#endif
