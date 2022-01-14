#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<bitset>

#include"Libraries/CompressionLib/bitio.h" //used to output in binary
//#include"Libraries/CompressionLib/huffman.h"//most of the huffman code

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24)) 

//huffman algorithm
//nodes in the tree
struct node{
char character;      //what character is represented
int count;           //count of the character/characters
int child0;          //location in vector of 0 node
int child1;          //location in vector of 1 node
int parent = -1;          //location in vector of parent node
bool used;        //whether node has been used
int code;
int bits; //number of bits in the code actually used
};
 std::vector<node> nodes;
    node dummynode;


//print arguments for debug
void PrintArguments(int argc, char* argv[]){
    for(int i = 0; i < argc; i++){
        std::cout << argv[i] << std::endl;
    }
}

void Generatecode(int parent){
    //leaf check
if(nodes.at(parent).character != char(7)){
    return;
}
//zero branch
nodes.at(nodes.at(parent).child0).code = nodes.at(parent).code << 1;
nodes.at(nodes.at(parent).child0).bits = nodes.at(parent).bits + 1;
//one branch
nodes.at(nodes.at(parent).child1).code = nodes.at(parent).code << 1;
nodes.at(nodes.at(parent).child1).code = nodes.at(nodes.at(parent).child1).code | 1;
nodes.at(nodes.at(parent).child1).bits = nodes.at(parent).bits + 1;

Generatecode(nodes.at(parent).child0);
Generatecode(nodes.at(parent).child1);
}


//usage: ./Encoder [Filename]
int main(int argc, char* argv[]){
    //incorrect usage
    if(argc != 2){
    std::cout << "incorrect usage\nusage: Encoder [Filename]\n";
    return -1;
    }

    PrintArguments(argc, argv);
    //ingest file
    std::ifstream input(argv[1], std::ifstream::in);
   if(input.is_open()){
       bitFILE* outputFile= bitIO_open("CompressedData.bin", BIT_IO_W);
        //get file length
       input.seekg(0,input.end);
       int length = input.tellg();
       input.seekg(0, input.beg);

        //allocate space for text
       char* Text = new char[length];
        //intake file and convert to string for manipulation
       input.read(Text,length);
       std::string InputData = Text;
       delete[] Text;
       std::cout << InputData << std::endl;
        //close filestream
       input.close();
    
    //build tree, generate codes and output counts to bin file
    //get counts
    //vector represents tree
   
      dummynode.count = 1; dummynode.child0 = 0; dummynode.child1 = 0;
    bool Char_Exists = false;
    //loop to get counts of characters
    for(int i = 0; i < InputData.length(); i++){
        Char_Exists = false;
        for(int j = 0;j < nodes.size(); j++){
            if(InputData.at(i) == nodes.at(j).character){
                nodes.at(j).count++;
                Char_Exists = true;
            }
        }
      
        if(Char_Exists == false){
             dummynode.character = InputData.at(i);
             nodes.push_back(dummynode);
             }    
    }
  
    //sort counts(bubble sort, highest first)
    for(int i = 0; i < nodes.size(); i++){
        for(int j = 0; j < nodes.size() - 1; j++){
            if(nodes.at(j).count < nodes.at(j+1).count){
                std::swap(nodes.at(j), nodes.at(j+ 1));
            }
        }
    }
   
   
 //print sorted characters with counts(the strangenes at the end with the test file is just carriage return being printed)
    for(int i = 0; i < nodes.size(); i++){
    std::cout << nodes.at(i).character << " " << nodes.at(i).count << std::endl;
    }
     int character_count = 0;
    for(int i = 0; i < nodes.size();i++){
        character_count = character_count + nodes.at(i).count;
    }
     std::cout << character_count << std::endl;
    std::cout << "no. of nodes "<<nodes.size() << std::endl;
    //output 
    //amount of nodes is always (no. of seperate characters) * 2 - 1 so lets exploit that as an end condtition
    int original_size = nodes.size();
    int stringsize = InputData.size();
    //output characters with their counts to binary file, first a count of nodes so the decoder knows how much it needs to take in
    
    //amount of characters
   

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

    std::cout << nodes.size() << std::endl;
   
   std::cout << InputData.size() << std::endl;
    //generate codes(walk down tree)
    nodes.at(nodes.size() - 1).bits = 0;
    Generatecode(nodes.size() - 1);
     for(int i = 0; i < nodes.size(); i++){
    std::cout << "node: "<< i << " " << nodes.at(i).character << " " << nodes.at(i).count << " " << nodes.at(i).parent << " " << nodes.at(i).child0 << " " << nodes.at(i).child1 <<" " << nodes.at(i).used  << " " << std::bitset<32>(nodes.at(i).code) << " " << nodes.at(i).bits <<std::endl;
    }


     bitIO_write(outputFile,&original_size,8);
    
    for(int i = 0; i < original_size; i++){
        bitIO_write(outputFile,&nodes.at(i).character,8);
        bitIO_write(outputFile,&nodes.at(i).count, 8);
    }
  
    //compress file and append compressed data to bin file
    for(int i = 0; i < InputData.size(); i++){
        for(int j = 0; j < original_size; j++){
            if(InputData.at(i) == nodes.at(j).character){
              
                bitIO_write(outputFile,&nodes.at(j).code,nodes.at(j).bits);
            }
        }
   }
   

 
        //cleanup
        bitIO_close(outputFile);
       

   } else {std::cout << "File could not be opened\n"; return -1;}
    

    return 0;
}