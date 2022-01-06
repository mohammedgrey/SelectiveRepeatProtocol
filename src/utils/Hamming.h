#ifndef HAMMING_H_
#define HAMMING_H_
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <bitset>
#include <cmath>
#include <string>
#include <sstream>

using namespace std;

inline string getBinaryStringFromPayLoad(string payload)
{
    string binaryString = "";
    for (int i = 0; i < payload.size(); i++)
    {
        bitset<8> charBits(payload[i]);
        binaryString += charBits.to_string();
    }
    return binaryString;
}

inline int getNewLength(string binaryPayLoad, int& r){
int m = binaryPayLoad.size();
// int r=0;
while( (pow(2.0,r)-r) < m+1){
    r=r+1;
}
int n = m+r; //new length
return n;
}

// ------------------------------------ Calculating Parities ------------------------------------------------- //

inline string getMsgWithParity(string binaryPayLoad){
int r=0;
int n = getNewLength(binaryPayLoad, r);
int msgWithParity[n+1];
std::stringstream msg;
int payLoadIndex=0;
for(int i=0; i<r; i++){ //use -1 to indicate places of parity bits
int parityPos=pow(2,i);
msgWithParity[parityPos]=-1;
}
for(int i=1; i<=n; i++){ //Copy pay load bits to the non-parity places in the array
if(msgWithParity[i]!=-1){
    msgWithParity[i]=(int)binaryPayLoad[payLoadIndex]-48;
    payLoadIndex++;
}
}
 for(int i=0; i<=n; i++){ //Calculate value of each parity bit
   int counter=0;
  // int parityPos=pow(2,i);
  if(msgWithParity[i]==-1) //indicates a parity
  {
    if(i==1){ //Special case for the first parity (2^0)
      int parity=0;
      msgWithParity[i]=0; //initialize the parity position with zero
      for(int j=i;j<=n;j+=2){
          parity+=msgWithParity[j];
      }
      if(parity%2==0){ //even number of ones
          msgWithParity[i]=0;
      }
      else{ //odd number of ones
          msgWithParity[i]=1;
      }
    }
    else{ //General case for the rest of the parities (2^i, i!=0)
       int j=i;
       int parity=0;
       msgWithParity[j]=parity; //initialize the parity position with zero
    while(j<=n)
    {
       parity+=msgWithParity[j];
       counter++;
       j++;
       if(counter%i == 0){
           j+=i;
       }
    }
    if(parity%2==0){ //even number of ones
          msgWithParity[i]=0;
    }
    else{ //odd number of ones
          msgWithParity[i]=1;
    }
   }
 }
 }
for(int i=1;i<=n;i++){
msg << msgWithParity[i];
}
return msg.str();
}

// --------------------------------- Responsible for detection only -----------------------------//

inline bool validHamming(string payLoad){
int r=0;
int R=0;
string binaryPayload = getBinaryStringFromPayLoad(payLoad);
string msgWithParity = getMsgWithParity(binaryPayload);
int n = getNewLength(binaryPayload,R);
int msgArr[n+1];
int countCorrectParity=0;
int index=0;
for(int i=1; i<=n; i++){
    msgArr[i]=(int)msgWithParity[index]-48;
    index++;
}
for(int i=0; i<=n; i++){
  //int parityPos=pow(2,r);
  int counter=0;
  if(i==pow(2,r)) //indicates a parity
  {
    if(pow(2,r)==1){ //Special case for the first parity (2^0)
      int parity=0;
      for(int j=i;j<=n;j+=2){
          parity+=msgArr[j];
      }
      if(parity%2==0){ //even number of ones --> parity=0
          countCorrectParity++;
      }
    }
    else{ //General case for the rest of the parities (2^i, i!=0)
       int j=i;
       int parity=0;
    while(j<=n)
    {
       parity+=msgArr[j];
       counter++;
       j++;
       if(counter%i == 0){
           j+=i;
       }
    }
    if(parity%2==0){ //even number of ones --> parity=0
          countCorrectParity++;
    }
   }
   r++;
 }
}
if(countCorrectParity==r){ //all parities are equal to zero
return true;
}
return false;
}

// ----------------------------------- Responsible for detection AND correction ------------------------------ //

inline string doHamming(string payLoad, int &errIndex){
int R=0;
string binaryPayload = getBinaryStringFromPayLoad(payLoad);
string msgWithParity = getMsgWithParity(binaryPayload);
int n = getNewLength(binaryPayload,R);
std::stringstream msg;
int r=0;
int msgArr[n+1];
int indexToCorrect=0;
int index=0;
for(int i=1; i<=n; i++){
    msgArr[i]=(int)msgWithParity[index]-48;
    index++;
}
for(int i=0; i<=n; i++){
 // int parityPos=pow(2,r);
  int counter=0;
  if(i==pow(2,r)) //indicates a parity
  {
    if(pow(2,r)==1){ //Special case for the first parity (2^0)
      int parity=0;
      for(int j=i;j<=n;j+=2){
          parity+=msgArr[j];
      }
      if(parity%2!=0){ //odd number of ones --> parity=1
          indexToCorrect+=i;
    }
    }
    else{ //General case for the rest of the parities (2^i, i!=0)
       int j=i;
       int parity=0;
    while(j<=n)
    {
       parity+=msgArr[j];
       counter++;
       j++;
       if(counter%i == 0){
           j+=i;
       }
    }
    if(parity%2!=0){ //odd number of ones --> parity=1
          indexToCorrect+=i;
    }
   }
   r++;
 }
}
msgArr[indexToCorrect]=!msgArr[indexToCorrect]; //modify the errored bit
errIndex=indexToCorrect;
for(int i=1;i<=n;i++){
msg << msgArr[i];
}
return msg.str();
}
#endif
