#include<iostream>
#include"Libraries/CompressionLib/bitio.h"
#include<vector>
#include<string>
#include<bitset>
#include<algorithm>
//#include"Libraries/CompressionLib/huffman.h"

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
 std::vector<node> nodes;
    node dummynode;

int main(int argc, char* argv[]){
bitFILE* inputbinary = bitIO_open("CompressedData.bin", BIT_IO_R);
std::string text;
int textlength;
int nodesamount;

bitIO_read(inputbinary,&nodesamount,sizeof(nodesamount),8);

nodes.resize(0);
for(int i = 0; i < nodesamount; i++){
bitIO_read(inputbinary,&dummynode.character, sizeof(dummynode.character),8);
bitIO_read(inputbinary,&dummynode.count, sizeof(dummynode.count),8);
nodes.push_back(dummynode);
}

for(int i = 0; i < nodesamount; i++){
    std::cout << nodes.at(i).character << " " << nodes.at(i).count << std::endl;
}
//build tree again
//build tree (character for a 'parent node' is always ASCII code 0 which will be used later to determine if we have reached the end of the tree when making the codes)
     int lowest1, lowest2; //addresses of lowest values
    //set some values just to be sure
     for(int i = 0; i < nodes.size(); i++){
         nodes.at(i).used = false;
         nodes.at(i).parent = -1;
         
     }
      //tree build loop
    while(nodes.size() < (nodesamount*2)-1){
        
        //identify two lowest weights
        //set begin values
       lowest1 = nodes.size() -1;
      lowest2 = nodes.size() - 1;
      //actually find lowest value
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

     for(int i = 0; i < nodes.size(); i++){
    std::cout << "node: "<< i << " " << nodes.at(i).character << " " << nodes.at(i).count << " " << nodes.at(i).parent << " " << nodes.at(i).child0 << " " << nodes.at(i).child1 <<" " << nodes.at(i).used  << " " << std::bitset<32>(nodes.at(i).code) << std::endl;
    }
//input data and make it readable
int bit = 2;
//ingest data
std::string binarystring;
std::string characterstring;
characterstring.resize(0);
char bitcharacter;
while(bitIO_feof(inputbinary) == 0){
  bitIO_read(inputbinary,&bit, sizeof(bit), 1);
  bitcharacter = '0' + bit;
  binarystring.append(1,bitcharacter);
}
bitIO_close(inputbinary);
//output for debug
std::cout << binarystring << std::endl;
std::reverse(binarystring.begin(), binarystring.end());
std::cout << binarystring << std::endl;
//convert to characters
int CurrentNode = nodes.size() - 1;
for(int i = 0; i < binarystring.size();i++){
  
if(binarystring.at(i) == '1'){
 // std::cout << "1";
  CurrentNode = nodes.at(CurrentNode).child1;
}
else if(binarystring.at(i) == '0'){
 // std::cout << "0";
  CurrentNode = nodes.at(CurrentNode).child0;
}
if(nodes.at(CurrentNode).character != (char)7){
    characterstring.append(1,nodes.at(CurrentNode).character);
    CurrentNode = nodes.size() - 1;
  }
}

std::reverse(characterstring.begin(), characterstring.end());
std::cout << characterstring << std::endl;
std::cin.get();
    return 0;
}

