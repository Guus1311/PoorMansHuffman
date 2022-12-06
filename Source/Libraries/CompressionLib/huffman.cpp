#include"huffman.h"

void PrintArguments(int argc, char* argv[]){
    for(int i = 0; i < argc; i++){
        std::cout << argv[i] << std::endl;
    }
}

void IngestFile(char* filename){
     std::ifstream input(filename, std::ifstream::in);
   if(input.is_open()){
     
        //get file length
       input.seekg(0,input.end);
       int Filelength = input.tellg();
       input.seekg(0, input.beg);

        //allocate space for text
       char* Text = new char[Filelength];
        //intake file and convert to string for manipulation
       input.read(Text,Filelength);
    
       delete[] Text;
       std::cout << InputData << std::endl;
        //close filestream
       input.close();
       
}
}
//counts occurences of every character and sets original size
void Countchars(){
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
    original_size = nodes.size();
}
//sorts by count high to low
void SortChars(){
for(int i = 0; i < nodes.size(); i++){
        for(int j = 0; j < nodes.size() - 1; j++){
            if(nodes.at(j).count < nodes.at(j+1).count){
                std::swap(nodes.at(j), nodes.at(j+ 1));
            }
        }
    }
}
//outputs counts and chars to binary file
void WriteCountsChars(){
    
     bitIO_write(binaryFile,&original_size,8);
    for(int i = 0; i < nodes.size(); i++){
        bitIO_write(binaryFile,&nodes.at(i).character,8);
        bitIO_write(binaryFile,&nodes.at(i).count, 8);
    }
}

void buildTree(){
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
}

void BinaryOpen(int mode){
    if(mode == 1 | mode == 0){
   binaryFile = bitIO_open("CompressedData.bin", mode);
    }
    else{std::cout << "Error: Incorrect filemode specified\n";}
}

void BinaryClose(){
    bitIO_close(binaryFile);
}

void PrintNodes(){
   std::cout << nodes.size() << std::endl;
    for(int i = 0; i < nodes.size(); i++){
    std::cout << "node: "<< i << " " << nodes.at(i).character << " " << nodes.at(i).count << " " << nodes.at(i).parent << " " << nodes.at(i).child0 << " " << nodes.at(i).child1 <<" " << nodes.at(i).used << std::endl;
    }
}

void GenerateCodes(){

}