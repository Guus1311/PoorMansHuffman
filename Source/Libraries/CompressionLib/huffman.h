#include<iostream>
#include<fstream>
#include<vector>
#include<string>

#include"bitio.h" //used to output in binary

//variables
struct node{
char character;      //what character is represented
int count;           //count of the character/characters
int child0;          //location in vector of 0 node
int child1;          //location in vector of 1 node
int parent = -1;          //location in vector of parent node
bool used;        //whether node has been used
long code;
};
std::vector<node> nodes;
node dummynode;

std::string InputData;
int original_size;

bitFILE* binaryFile;

//usage: ./Encoder [Filename]



//print arguments for debug
void PrintArguments(int argc, char* argv[]);

void IngestFile(char* filename);

void CountChars();

void SortChars();

void WriteCountsChars();

void buildTree();

void BinaryOpen(int mode);

void BinaryClose();

void PrintNodes();

void Generatecodes();


