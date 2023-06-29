// reading a text file
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <bitset>
using namespace std;
int  correctPred=0;
int  ncorrectPred=0;
bitset<2> bhr=00;

vector<bitset<2>> ptable(65536,10);
vector<bitset<2>> bhrTable(4,01);
vector<bitset<2>> addtable(16384,00);
bool hispredictor(uint32_t address,bitset<2> history){
        bitset<2> out;
        if(history==00){
           out = bhrTable[0]; 
        }
        else if(history==01){
          out = bhrTable[1]; 
       }
       else if(history==10){
          out = bhrTable[2]; 
       }
       else{
          out = bhrTable[3]; 
       }

       if(out.to_ulong()>=2){
          return true;
       }
       return false;
}     

void histrain(uint32_t address,bitset<2> history,bool outcome)  {
       int zero =0;
       unsigned long zer = zero;
       int three = 3;
       unsigned long th = three;
       if(history==00){
           if(outcome){
              bitset<2> m= bhrTable[0];
              unsigned long a =m.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2>  bits(b);
               bhrTable[0]= bits;
           }
           else{
               bitset<2> m= bhrTable[0];
               unsigned long a =m.to_ulong();
               unsigned long b = max(zer,a-1);
               bitset<2>  bits(b);
               bhrTable[0]= bits;
           }  
       }
       else if(history==01){
         if(outcome){
              bitset<2> m= bhrTable[1];
              unsigned long a = m.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2>  bits(b);
               bhrTable[1]= bits;
           }
           else{
               bitset<2> m= bhrTable[1];
               unsigned long a = m.to_ulong();
               unsigned long b = max(zer,a-1);
               bitset<2>  bits(b);
               bhrTable[1]= bits;
           } 
       }
       else if(history==10){
           if(outcome){
              bitset<2> m= bhrTable[2];
              unsigned long a = m.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2>  bits(b);
               bhrTable[2]= bits;
           }
           else{
               bitset<2> m= bhrTable[2];
               unsigned long a =m.to_ulong();
               unsigned long b = max(zer,a-1);
               bitset<2>  bits(b);
               bhrTable[2]= bits;
           }  
       }
       else{
           if(outcome){
              bitset<2> m= bhrTable[3];
              unsigned long a = m.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2>  bits(b);
               bhrTable[3]= bits;
           }
           else{
               bitset<2> m= bhrTable[3];
               unsigned long a = m.to_ulong();
               unsigned long b = max(zer,a-1);
               bitset<2>  bits(b);
               bhrTable[3]= bits;
           } 
       }
      if(outcome){
               unsigned long  a = history.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2> bits(b);
               bhr = bits;
       }
       else{
               unsigned long  a = history.to_ulong();
               unsigned long b = max(zer,a-1);
               bitset<2> bits(b);
               bhr = bits;
       }

}

bool addpredictor(uint32_t address,bitset<2> history){
      int lastNBits = address & ((1 << 14) - 1);
      bitset<14> check = lastNBits;
      unsigned long ind = check.to_ulong();
      bitset<2> ans = addtable[ind];
      if(ans.to_ulong()>=2){
             return true;  
       }
       
       return false;

}  

void addtrain(uint32_t address,bitset<2> prev_history,bool outcome)  {
      int lastNBits = address & ((1 << 14) - 1);
      bitset<14> check = lastNBits;
      unsigned long ind = check.to_ulong();
        if(outcome){
              bitset<2> ans = addtable[ind];
              unsigned long m =  ans.to_ulong();
              int three = 3;
              unsigned long th = three;
              unsigned long a=min(th,m+1);
              bitset<2> bits(a);
              addtable[ind] = bits;
       }
       else{
             bitset<2> ans = addtable[ind];
              unsigned long m =  ans.to_ulong();
              int zer = 3;
              unsigned long z = zer;
             int a=max(z,m-1); 
             bitset<2> bits(a);
             addtable[ind] = bits;
       }
}

bool predictor(long address,bitset<2> history){
       int lastNBits = address & ((1 << 14) - 1);
       bitset<14> i = lastNBits;
       bitset<16> check;
       for(int o=0;o<14;o++){
              check[o]=i[o];
       }
     
      // cout<<history<<endl;

       if(history==00){
            check[14]=0;
            check[15]=0;
       }
       else if(history==01){
          check[14]=0;
          check[15]=1;
       }
       else if(history==10){
          check[14]=1;
          check[15]=0;
       }
       else{
          check[14]=1;
          check[15]=1;
       }
       unsigned long ind = check.to_ulong();
       
      // prev_his = history;
     
       if(ptable[ind].to_ulong()>=2){
             return true;  
       }
       
       return false;
}

void train(long address,bitset<2> prev_history,bool outcome)  {
       int lastNBits = address & ((1 << 14) - 1);
       bitset<14> i = lastNBits;
       bitset<16> check;
       for(int o=0;o<14;o++){
              check[o]=i[o];
       }
       if(prev_history==00){
            check[14]=0;
            check[15]=0;
       }
       else if(prev_history==01){
          check[14]=0;
          check[15]=1;
       }
       else if(prev_history==10){
          check[14]=1;
          check[15]=0;
       }
       else{
          check[14]=1;
          check[15]=1;
       }
       unsigned long ind = check.to_ulong(); 

       int zero =0;
       unsigned long zer = zero;
       int three = 3;
       unsigned long th = three;

       if(outcome){

              bitset<2> m= ptable[ind];
              unsigned long a =m.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2>  bits(b);
               ptable[ind]= bits;


             // ptable[ind]=min(3,ptable[ind]+1);
              unsigned long  a1 = bhr.to_ulong();
               unsigned long b1 = min(th,a1+1);
               bitset<2> bits1(b1);
               bhr = bits1;
       }
       else{
            
            bitset<2> m= ptable[ind];
              unsigned long a =m.to_ulong();
               unsigned long b = max(zer,a+1);
               bitset<2>  bits(b);
               ptable[ind]= bits;

             //ptable[ind]=max(0,ptable[ind]-1); 
             unsigned long  a1 = bhr.to_ulong();
               unsigned long b1 = max(zer,a1-1);
               bitset<2> bits1(b1);
               bhr = bits1;
       }
}

int main(int argc, char *argv[])
{
  ifstream myfile (argv[1]);
  int choose = stoi(argv[2]);
  string line;
  if (myfile.is_open())
  {
   if(choose==1){
     while ( getline (myfile,line) )
     {  
        string pc="";
        bool branch;
        for(int i=0;i<=8;i++){
            pc+=line[i];
        }    
         unsigned int value = std::stoul(pc, nullptr, 16);
         branch = line[9];
         bool predict=hispredictor(value,bhr);
         if(predict==branch){
            correctPred++;
         }
         else{
            ncorrectPred++;
         }

         histrain(value, bhr, branch);
         }
      cout<<correctPred<<endl;
      cout<<ncorrectPred<<endl;
      myfile.close();
   }
   if(choose==2){
     while ( getline (myfile,line) )
     {  
        string pc="";
        bool branch;
        for(int i=0;i<=8;i++){
            pc+=line[i];
        }    
         unsigned int value = std::stoul(pc, nullptr, 16);
         branch = line[9];
         bool predict=addpredictor(value,bhr);
         if(predict==branch){
            correctPred++;
         }
         else{
            ncorrectPred++;
         }

         addtrain(value, bhr, branch);
         }
      cout<<correctPred<<endl;
      cout<<ncorrectPred<<endl;
      myfile.close();
   }
   
   if(choose==3){
     while ( getline (myfile,line) )
     {  
        string pc="";
        bool branch;
        for(int i=0;i<=8;i++){
            pc+=line[i];
        }    
         unsigned int value = std::stoul(pc, nullptr, 16);
         branch = line[9];
         bool predict=predictor(value,bhr);
         if(predict==branch){
            correctPred++;
         }
         else{
            ncorrectPred++;
         }

         train(value, bhr, branch);
         }
      cout<<correctPred<<endl;
      cout<<ncorrectPred<<endl;
      myfile.close();
   }
   

  }

  else cout << "Unable to open file";

  return 0;
}



