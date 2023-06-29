#ifndef __BRANCH_PREDICTOR_HPP__
#define __BRANCH_PREDICTOR_HPP__

#include <vector>
#include <bitset>
#include <cassert>

struct BranchPredictor {
    virtual bool predict(uint32_t pc) = 0;
    virtual void update(uint32_t pc, bool taken) = 0;
};

struct SaturatingBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> table;
    SaturatingBranchPredictor(int value) : table(1 << 14, value) {}

    bool predict(uint32_t pc) {
         int lastNBits = pc & ((1 << 14) - 1);
         bitset<14> check = lastNBits;
         unsigned long ind = check.to_ulong();
         bitset<2> ans = table[ind];
         if(ans.to_ulong()>=2){
             return true;  
            }
         return false;
        
    }

    void update(uint32_t pc, bool taken) {
        int lastNBits = pc & ((1 << 14) - 1);
        bitset<14> check = lastNBits;
        unsigned long ind = check.to_ulong();
        if(taken){
              bitset<2> ans = table[ind];
              unsigned long m =  ans.to_ulong();
              int three = 3;
              unsigned long th = three;
              unsigned long a=min(th,m+1);
              bitset<2> bits(a);
              table[ind] = bits;
       }
       else{
              bitset<2> ans = table[ind];
              unsigned long m =  ans.to_ulong();
              int zer = 3;
              unsigned long z = zer;
             int a=max(z,m-1); 
             bitset<2> bits(a);
             table[ind] = bits;
       }
    }
};

struct BHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    BHRBranchPredictor(int value) : bhrTable(1 << 2, value), bhr(value) {}

    bool predict(uint32_t pc) {
        // your code here

        bitset<2> out;
        if(bhr==00){
           out = bhrTable[0]; 
        }
        else if(bhr==01){
          out = bhrTable[1]; 
       }
        else if(bhr==10){
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

    void update(uint32_t pc, bool taken) {
        // your code here
       int zero =0;
       unsigned long zer = zero;
       int three = 3;
       unsigned long th = three;
       if(bhr==00){
           if(taken){
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
       else if(bhr==01){
         if(taken){
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
       else if(bhr==10){
           if(taken){
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
           if(taken){
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
      if(taken){
               unsigned long  a = bhr.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2> bits(b);
               bhr = bits;
       }
       else{
               unsigned long  a = bhr.to_ulong();
               unsigned long b = max(zer,a-1);
               bitset<2> bits(b);
               bhr = bits;
       }

    }
};

struct SaturatingBHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    std::vector<std::bitset<2>> table;
    std::vector<std::bitset<2>> combination;
    SaturatingBHRBranchPredictor(int value, int size) : bhrTable(1 << 2, value), bhr(value), table(1 << 14, value), combination(size, value) {
        assert(size <= (1 << 16));
    }

    bool predict(uint32_t pc) {
        // your code here


       int lastNBits = pc & ((1 << 14) - 1);
       bitset<14> i = lastNBits;
       bitset<16> check;
       for(int o=0;o<14;o++){
              check[o]=i[o];
       }
     
      // cout<<bhr<<endl;

       if(bhr==00){
            check[14]=0;
            check[15]=0;
       }
       else if(bhr==01){
          check[14]=0;
          check[15]=1;
       }
       else if(bhr==10){
          check[14]=1;
          check[15]=0;
       }
       else{
          check[14]=1;
          check[15]=1;
       }
       unsigned long ind = check.to_ulong();
       
      // prev_his = bhr;
     
       if(combination[ind].to_ulong()>=2){
             return true;  
       }
       
       return false;
    }

    void update(uint32_t pc, bool taken) {
       int lastNBits = pc & ((1 << 14) - 1);
       bitset<14> i = lastNBits;
       bitset<16> check;
       for(int o=0;o<14;o++){
              check[o]=i[o];
       }
       if(bhr==00){
            check[14]=0;
            check[15]=0;
       }
       else if(bhr==01){
          check[14]=0;
          check[15]=1;
       }
       else if(bhr==10){
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

       if(taken){

              bitset<2> m= combination[ind];
              unsigned long a =m.to_ulong();
               unsigned long b = min(th,a+1);
               bitset<2>  bits(b);
               combination[ind]= bits;


             // combination[ind]=min(3,combination[ind]+1);
              unsigned long  a1 = bhr.to_ulong();
               unsigned long b1 = min(th,a1+1);
               bitset<2> bits1(b1);
               bhr = bits1;
       }
       else{
            
             bitset<2> m= combination[ind];
              unsigned long a =m.to_ulong();
               unsigned long b = max(zer,a+1);
               bitset<2>  bits(b);
               combination[ind]= bits;

             //combination[ind]=max(0,combination[ind]-1); 
             unsigned long  a1 = bhr.to_ulong();
               unsigned long b1 = max(zer,a1-1);
               bitset<2> bits1(b1);
               bhr = bits1;
       }
    }
};

#endif