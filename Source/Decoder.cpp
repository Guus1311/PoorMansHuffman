#include<iostream>
#include"Libraries/CompressionLib/bitio.h"
#include<vector>
#include<string>
#include<bitset>
#include<algorithm>
#include<fstream>
//#include"Libraries/CompressionLib/huffman.h"

//variables

std::string TextFilename = "TextOutput.txt";
bool DebugMode = false;


//huffman algorithm
//nodes in the tree
struct node{
char character;      //what character is represented
int count;           //count of the character/characters
int child0;          //location in vector of 0 node
int child1;          //location in vector of 1 node
int parent = -1;          //location in vector of parent node
bool used;        //whether node has been used
long code;
};
//tree 
 std::vector<node> nodes;
 //used to prepare new nodes for insertion in vector
    node dummynode;


    //processing of commandline arguments
    void ProcessArguments(int argc, char* argv[]){
      //if only one argument set argument as output file
      if(argc == 2){
        TextFilename = argv[1];
      }
       //special conditions
      for(int i = 0; i < argc; i++){
        //options
        if(argv[i][0] == '-'){
          //debug mode
          if(argv[i][1] == 'd'){
          std::cout << "Debug\n";
          DebugMode = true;
          }
          //set output file
          if(argv[i][1] == 'o'){
           TextFilename = argv[i+1];
          }
          //help
        if(argv[i][1] == 'h'){
          std::cout << "Help\n";
          std::cout << "Usage ./Decoder[options] -o [output]\n./Decoder [output]\n./Decoder\n";
          }
        }
        
       }
      
    }

int main(int argc, char* argv[]){
  //process commandline arguments
  ProcessArguments(argc,  argv);

  //variables
  //bitfile
bitFILE* inputbinary = bitIO_open("CompressedData.bin", BIT_IO_R);
//string text comes in
std::string text;
//same usage as in encoder
int original_size;
//get orginal size
bitIO_read(inputbinary,&original_size,sizeof(original_size),8);
//make nodes 0 long
nodes.resize(0);
//get character and counts for all leaf nodes
for(int i = 0; i < original_size; i++){
bitIO_read(inputbinary,&dummynode.character, sizeof(dummynode.character),8);
bitIO_read(inputbinary,&dummynode.count, sizeof(dummynode.count),8);
nodes.push_back(dummynode);
}
//print leaf nodes
if(DebugMode == true){
for(int i = 0; i < original_size; i++){
    std::cout << nodes.at(i).character << " " << nodes.at(i).count << std::endl;
}
}
//build tree again(copided from encoder so little more detailed comments there)
//build tree (character for a 'parent node' is always ASCII code 0 which will be used later to determine if we have reached the end of the tree when making the codes)
     int lowest1, lowest2; //addresses of lowest values
    //set some values just to be sure
     for(int i = 0; i < nodes.size(); i++){
         nodes.at(i).used = false;
         nodes.at(i).parent = -1;
         
     }
      //tree build loop
    while(nodes.size() < (original_size*2)-1){
        
        //identify two lowest weights
        //set begin values
       lowest1 = nodes.size() -1;
      lowest2 = nodes.size() - 1;
      //find lowest value
        for(int i = nodes.size() - 1; i > -1; i--){
            if(nodes.at(i).used == false){
         if(nodes.at(i).count <= nodes.at(lowest1).count){
           
              lowest1 = i;
           }
            
         }
       }
       //find second lowest value
         for(int i = nodes.size() - 1; i > -1; i--){
         if(nodes.at(i).count <= nodes.at(lowest2).count){
              if(nodes.at(i).used == false){
            if(i != lowest1){
              
              lowest2 = i;
               }
             }
         }
       }
       //if no lower or equel value can be found for lowest2, chances are the next one over is still free(mostly for root node)
       if(lowest1 == lowest2){
           if(nodes.at(lowest1 - 1).used == false){
               lowest1--;
           }
       }

        //create new parent node and set its values. values get put into the dummy node and then put at the end of the vector
        dummynode.character = (char)7; //bell
        dummynode.count = nodes.at(lowest1).count + nodes.at(lowest2).count;
        dummynode.child0 = lowest1;
        dummynode.child1 = lowest2;
        dummynode.used = false;
        nodes.push_back(dummynode);
       // set values for child nodes, the new parent is always the latest node 
       nodes.at(lowest1).parent = nodes.size() - 1;
       nodes.at(lowest2).parent = nodes.size() - 1;
       //this way nodes dont get reused
       nodes.at(lowest1).used = true;
       nodes.at(lowest2).used = true;
    }
    //output tree for debug
    if(DebugMode == true){
     for(int i = 0; i < nodes.size(); i++){
    std::cout << "node: "<< i << " " << nodes.at(i).character << " " << nodes.at(i).count << " " << nodes.at(i).parent << " " << nodes.at(i).child0 << " " << nodes.at(i).child1 <<" " << nodes.at(i).used  << " " << std::bitset<32>(nodes.at(i).code) << std::endl;
    }
    }

//input data and make it readable
//set int to 2 so we know if it doesnt read at all
int bit = 2;
//ingest data
//string that holds the read binary
std::string binarystring;
//text file
std::string characterstring;
//default just in case
characterstring.resize(0);
//for conversion and appending bit
char bitcharacter;
while(bitIO_feof(inputbinary) == 0){
  //read bit, convert then append it
  bitIO_read(inputbinary,&bit, sizeof(bit), 1);
  bitcharacter = '0' + bit;
  binarystring.append(1,bitcharacter);
}
//file is read so we close it
bitIO_close(inputbinary);
//output for debug

//reverse binarystring because it gets read backwards or something(endianness maybe?) for now this fixes it
std::reverse(binarystring.begin(), binarystring.end());
if(DebugMode == true){
  std::cout << binarystring << std::endl;
}
//convert to characters
int CurrentNode = nodes.size() - 1;
for(int i = 0; i < binarystring.size();i++){
  //we have the tree so we just walk down it the duraction fo the string
if(binarystring.at(i) == '1'){
 // std::cout << "1";
  CurrentNode = nodes.at(CurrentNode).child1;
}
else if(binarystring.at(i) == '0'){
 // std::cout << "0";
  CurrentNode = nodes.at(CurrentNode).child0;
}
//check if we are on a leaf node and if so append the leaf's character to our text string
if(nodes.at(CurrentNode).character != (char)7){
    characterstring.append(1,nodes.at(CurrentNode).character);
    CurrentNode = nodes.size() - 1;
  }
}
//because we reverse the binary our text is reversed so we reverse it back
std::reverse(characterstring.begin(), characterstring.end());

//output to text file
//be able to take in filename but have backup

//output
std::ofstream TextOutput(TextFilename, std::ofstream::out);
if(TextOutput.is_open()){
TextOutput << characterstring;
}
TextOutput.close();
std::cin.get();
    return 0;
}

