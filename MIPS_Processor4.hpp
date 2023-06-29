/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 * 
 */

#ifndef __MIPS_PROCESSOR_HPP__
#define __MIPS_PROCESSOR_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>
#include "IF.hpp"
#include "ID.hpp"
#include "EX.hpp"
#include "MEM.hpp"
#include "WB.hpp"

#include <bits/stdc++.h>


using namespace std;

struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr = 0, PCnext;
	std::unordered_map<std::string, std::function<int(MIPS_Architecture &, std::string, std::string, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;

	int temp=0;
	int temp_address=0;

		/// for sw address value changed or not
	bool if_address__value_change=false;
	int   that_address_changed=0;
	int that_vaue_changed=0;


		//IFstage->PCcurr=0; 
	map<string,int> reg_to_be_updated;
		//reg_to_be_updated[reg]=0 //updated and present in L3 i.e main register memeory register
		//                      =1 //not updated
		//                      =4  // updated and present in L4
		//                      =5  //updated adn present in L5
	map<string,int> L4;
	map<string, int> L5;
	map<string, int> L6;



	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};

	// constructor to initialise the instruction set
	MIPS_Architecture(std::ifstream &file)
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, 
		{"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi},
		{"sll", &MIPS_Architecture::sll}, {"srl", &MIPS_Architecture::srl},
		{"and", &MIPS_Architecture::my_and}, {"or", &MIPS_Architecture::my_or},{"xor", &MIPS_Architecture::my_xor}};

		for (int i = 0; i < 32; ++i)
			registerMap["$" + std::to_string(i)] = i;

		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + std::to_string(i)] = i + 4;
		
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + std::to_string(i)] = i + 8, registerMap["$s" + std::to_string(i)] = i + 16;
		
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;

		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

	// perform add operation
	int add(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a + b; });
	}

	// perform subtraction operation
	int sub(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a - b; });
	}

	// perform multiplication operation
	int mul(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a * b; });
	}


	int my_and(std::string r1, std::string r2, std::string r3){
		
		return op(r1, r2, r3, [&](int a, int b)
				  { return (a & b); });
	}


	int my_or(std::string r1, std::string r2, std::string r3){
		
		return op(r1, r2, r3, [&](int a, int b)
				  { return (a | b); });
	}

	int my_xor(std::string r1, std::string r2, std::string r3){
		
		return op(r1, r2, r3, [&](int a, int b)
				  { return (a^b); });
	}


	// perform the binary operation
	int op(std::string r1, std::string r2, std::string r3, std::function<int(int, int)> operation)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;

		//both r2 and r3 are present in register memory
		if(reg_to_be_updated[r2]==0 && reg_to_be_updated[r3]==0){
			temp = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
		}else if(reg_to_be_updated[r2]==5 && reg_to_be_updated[r3]==5){
			temp= operation(L5[r2], L5[r3]);
		}else if(reg_to_be_updated[r2]==6 && reg_to_be_updated[r3]==6){
			temp= operation(L6[r2], L6[r3]);
		}else if(reg_to_be_updated[r2]==5 && reg_to_be_updated[r3]==6){
			temp= operation(L4[r2], L5[r3]);
		}else if(reg_to_be_updated[r2]==6 && reg_to_be_updated[r3]==5){
			temp= operation(L6[r2], L5[r3]);
		}else if( (reg_to_be_updated[r2]==0 || reg_to_be_updated[r2]==1)  && reg_to_be_updated[r3]==5){
			temp = operation(registers[registerMap[r2]], L5[r3]);
		}else if((reg_to_be_updated[r3]==0 || reg_to_be_updated[r3]==1)  && reg_to_be_updated[r2]==5){
			temp = operation(L5[r2],registers[registerMap[r3]]);
		}else if( (reg_to_be_updated[r2]==0 || reg_to_be_updated[r2]==1)  && reg_to_be_updated[r3]==6){
			temp = operation(registers[registerMap[r2]], L6[r3]);
		}else if((reg_to_be_updated[r3]==0 || reg_to_be_updated[r3]==1)  && reg_to_be_updated[r2]==6){
			temp = operation(L6[r2],registers[registerMap[r3]] );
		}
		else{
			//case to resolve addi $1 ,$1 ,$2

			//we did reg_to_updated[$1]=1 in ID , but we reach to EX its means $1 is in register memory only
			temp = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
		}	
		
		//PCnext = PCcurr + 1;
		return 0;
	}

	// perform the beq operation
	int beq(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a == b; });
	}

	// perform the bne operation
	int bne(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a != b; });
	}

	// implements beq and bne by taking the comparator
	int bOP(std::string r1, std::string r2, std::string label, std::function<bool(int, int)> comp)
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		if (!checkRegisters({r1, r2}))
			return 1;
		//PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCcurr + 1;
		if(reg_to_be_updated[r1]==0 && reg_to_be_updated[r2]==0){
			PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCnext;
		}else if(reg_to_be_updated[r1]==5 && reg_to_be_updated[r2]==5){
			PCnext = comp(L5[r1], L5[r2]) ? address[label] : PCnext;

		}else if(reg_to_be_updated[r1]==6 && reg_to_be_updated[r2]==6){
			PCnext = comp(L6[r1], L6[r2]) ? address[label] : PCnext;

		}else if(reg_to_be_updated[r1]==5 && reg_to_be_updated[r2]==6){

				PCnext = comp(L5[r1], L6[r2]) ? address[label] : PCnext;

		}else if(reg_to_be_updated[r1]==6 && reg_to_be_updated[r2]==5){

					PCnext = comp(L6[r1], L5[r2]) ? address[label] : PCnext;
		}else{
			PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCnext;
		}

		return 0;
	}

	// implements slt operation
	int slt(std::string r1, std::string r2, std::string r3)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		

		//both r2 and r3 are present in register memory
		if(reg_to_be_updated[r2]==0 && reg_to_be_updated[r3]==0){
			temp = registers[registerMap[r2]] < registers[registerMap[r3]];
		}else if(reg_to_be_updated[r2]==5 && reg_to_be_updated[r3]==5){
			temp = L5[r2] < L5[r3];
		}else if(reg_to_be_updated[r2]==6 && reg_to_be_updated[r3]==6){
			temp = L6[r2] < L6[r3];
		}else if(reg_to_be_updated[r2]==5 && reg_to_be_updated[r3]==6){
			temp = L5[r2] < L6[r3];
		}else if(reg_to_be_updated[r2]==6 && reg_to_be_updated[r3]==5){
			temp = L6[r2] < L5[r3];
		}else{
			//case to resolve addi $1 ,$1 ,$2

			//we did reg_to_updated[$1]=1 in ID , but we reach to EX its means $1 is in register memory only
			temp = registers[registerMap[r2]] < registers[registerMap[r3]];
		}	
		
		
		//PCnext = PCcurr + 1;
		return 0;
	}

	// perform the jump operation
	int j(std::string label, std::string unused1 = "", std::string unused2 = "")
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		PCnext = address[label];
		return 0;
	}

	// perform load word operation
	int lw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r) || registerMap[r] == 0)
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);

		temp= data[address];
		//registers[registerMap[r]] = data[address];
		//PCnext = PCcurr + 1;
		return 0;
	}

	// perform store word operation
	int sw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);

		//in sw
		//we will update this addres in MEM stage only

		if(reg_to_be_updated[r]==0){
			data[address] = registers[registerMap[r]];
		}else if(reg_to_be_updated[r]==5){
			data[address] = L5[r];
		}else if(reg_to_be_updated[r]==6){
			data[address] = L6[r];
		}else{
			data[address] = registers[registerMap[r]];
		}
		
		if_address__value_change=true;
		that_address_changed=address;
		that_vaue_changed=data[address];

		//PCnext = PCcurr + 1;
		return 0;
	}

	int locateAddress(std::string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				std::string reg = location.substr(lparen + 1);
				reg.pop_back();
				if (!checkRegister(reg))
					return -3;
				
				int address=0;
				//if it is in memory i.e is being written in WB stage
				if(reg_to_be_updated[reg]==0){
					address = registers[registerMap[reg]] + offset;
				}else if(reg_to_be_updated[reg]==5){
					address = L5[reg] + offset;
				}else if(reg_to_be_updated[reg]==6){
					address = L6[reg] + offset;
					
				}else{
					address = registers[registerMap[reg]] + offset;
				}
				
				if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
					return -3;
				return address / 4;
			}
			catch (std::exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
				return -3;
			return address / 4;
		}
		catch (std::exception &e)
		{
			return -4;
		}
	}

	// perform add immediate operation
	int addi(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0)
			return 1;
		try
		{	
			if(reg_to_be_updated[r2]==0){
				temp = registers[registerMap[r2]] + stoi(num);
			}else if(reg_to_be_updated[r2]==5){
				temp = L5[r2] + stoi(num);
			}else if(reg_to_be_updated[r2]==6){
				temp = L6[r2] + stoi(num);
			}else{
				//case to resolve addi $1 ,$1 ,1

				//we did reg_to_updated[$1]=1 in ID , but we reach to EX its means $1 is in register memory only
				temp = registers[registerMap[r2]] + stoi(num);	
			}
			
			//PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}



	int sll(std::string r1, std::string r2, std::string num){
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0)
			return 1;
		try
		{
			if(reg_to_be_updated[r2]==0){
				temp = registers[registerMap[r2]];
				temp= (temp<<stoi(num));
			}else if(reg_to_be_updated[r2]==5){
				temp = L5[r2];
				temp= (temp<<stoi(num));
			}else if(reg_to_be_updated[r2]==6){
				temp = L6[r2];
				temp= (temp<<stoi(num));
			}else{
				//case to resolve addi $1 ,$1 ,1

				//we did reg_to_updated[$1]=1 in ID , but we reach to EX its means $1 is in register memory only
				temp = registers[registerMap[r2]];
				temp= (temp<<stoi(num));
			}
			

			
			//PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}


	int srl(std::string r1, std::string r2, std::string num){
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0)
			return 1;
		try
		{
			if(reg_to_be_updated[r2]==0){
				temp = registers[registerMap[r2]];
				temp= (temp>>stoi(num));
			}else if(reg_to_be_updated[r2]==5){
				temp = L5[r2];
				temp= (temp>>stoi(num));
			}else if(reg_to_be_updated[r2]==6){
				temp = L6[r2];
				temp= (temp>>stoi(num));
			}else{
				//case to resolve addi $1 ,$1 ,1

				//we did reg_to_updated[$1]=1 in ID , but we reach to EX its means $1 is in register memory only
				temp = registers[registerMap[r2]];
				temp= (temp>>stoi(num));
			}
			
			//PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}



////////////////////----------------------////////////////////////////////
	// checks if label is valid
	inline bool checkLabel(std::string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c)
														   { return (bool)isalnum(c); }) &&
			   instructions.find(str) == instructions.end();
	}

	// checks if the register is a valid one
	inline bool checkRegister(std::string r)
	{
		return registerMap.find(r) != registerMap.end();
	}

	// checks if all of the registers are valid or not
	bool checkRegisters(std::vector<std::string> regs)
	{
		return std::all_of(regs.begin(), regs.end(), [&](std::string r)
						   { return checkRegister(r); });
	}

	/*
		handle all exit codes:
		0: correct execution
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(exit_code code, int cycleCount)
	{
		//std::cout << '\n';
		switch (code)
		{
		case 1:
			std::cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			std::cerr << "Label used not defined or defined too many times\n";
			break;
		case 3:
			std::cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			std::cerr << "Syntax error encountered\n";
			break;
		case 5:
			std::cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			std::cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				std::cerr << s << ' ';
			std::cerr << '\n';
		}
		/*
		std::cout << "\nFollowing are the non-zero data values:\n";
		for (int i = 0; i < MAX / 4; ++i)
			if (data[i] != 0)
				std::cout << 4 * i << '-' << 4 * i + 3 << std::hex << ": " << data[i] << '\n'
						  << std::dec;
		std::cout << "\nTotal number of cycles: " << cycleCount << '\n';
		std::cout << "Count of instructions executed:\n";
		for (int i = 0; i < (int)commands.size(); ++i)
		{
			std::cout << commandCount[i] << " times:\t";
			for (auto &s : commands[i])
				std::cout << s << ' ';
			std::cout << '\n';
		}

		*/
	}

	// parse the command assuming correctly formatted MIPS instruction (or label)
	void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
		{
			std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command.clear();
		}
		else if (command[0].back() == ':')
		{
			std::string label = command[0].substr(0, command[0].size() - 1);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command = std::vector<std::string>(command.begin() + 1, command.end());
		}
		else if (command[0].find(':') != std::string::npos)
		{
			int idx = command[0].find(':');
			std::string label = command[0].substr(0, idx);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command[0] = command[0].substr(idx + 1);
		}
		else if (command[1][0] == ':')
		{
			if (address.find(command[0]) == address.end())
				address[command[0]] = commands.size();
			else
				address[command[0]] = -1;
			command[1] = command[1].substr(1);
			if (command[1] == "")
				command.erase(command.begin(), command.begin() + 2);
			else
				command.erase(command.begin(), command.begin() + 1);
		}
		if (command.empty())
			return;
		if (command.size() > 4)
			for (int i = 4; i < (int)command.size(); ++i)
				command[3] += " " + command[i];
		command.resize(4);
		commands.push_back(command);
	}



	void parseCommand1(std::string line){
		int i=0;
		std::string s="";
		for(int j=0; j<line.size(); j++){

			if(line[j]==';'){
				s=line.substr(i,j-i);
				//cout<<s<<endl;
				parseCommand(s);
				i=j+1;
			}
		}
	
		s=line.substr(i,line.size());
			//cout<<s<<endl;
		parseCommand(s);
	}

	
	// construct the commands vector from the input file
	void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			//for assignment use parseCommand1 otherwise use parseCommand
			parseCommand1(line);
		file.close();
	}

	// execute the commands sequentially (no pipelining)
	void executeCommandsUnpipelined()
	{
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}

		int clockCycles = 0;
		while (PCcurr < commands.size())
		{
			++clockCycles;
			std::vector<std::string> &command = commands[PCcurr];
			if (instructions.find(command[0]) == instructions.end())
			{
				handleExit(SYNTAX_ERROR, clockCycles);
				return;
			}
			exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);
			if (ret != SUCCESS)
			{
				handleExit(ret, clockCycles);
				return;
			}
			++commandCount[PCcurr];
			PCcurr = PCnext;
			printRegisters(clockCycles);
		}
		handleExit(SUCCESS, clockCycles);
	}

		// execute the commands sequentially (pipelining)
	void executeCommandspipelined()
	{
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}

		IF *IFstage1 = new IF();
		IF *IFstage2 = new IF();

		ID *IDstage1 = new ID();
		ID *IDstage2 = new ID();
		ID *RRstage = new ID();
		
		EX *EXstage1 = new EX();

		EX *EXstage2 = new EX();

		MEM *MEMstage1 = new MEM();
		MEM *MEMstage2 = new MEM();

		WB *WBstage1 = new WB();
		WB *WBstage2 = new WB();


		// //IFstage->PCcurr=0; 
		// map<string,int> reg_to_be_updated;
		// //reg_to_be_updated[reg]=0 //updated and present in L3 i.e main register memeory register
		// //                      =1 //not updated
		// //                      =4  // updated and present in L4
		// //                      =5  //updated adn present in L5
		//                         =6  //updated and present in L6
		// map<string,int> L4;
		// map<string, int> L5;
		// map<string, int> L6;

		int clockCycles = 0;

		


		std::vector<std::string> command={}; 
		exit_code ret ;

		//for finding register used in memory calculations in lw and sw
		std::string tempreg="";

		int templeft=0;
		bool first_time=true;


		if_address__value_change=false;

		printRegisters(clockCycles);
		if(if_address__value_change==true){
			cout<<that_address_changed<<" "<<that_vaue_changed<<endl;
		}else{
			cout<<0<<endl;
		}


		while (clockCycles<20000)
		{

			if_address__value_change=false;
			
			
			 if(WBstage1->stage_empty==false){
			 	WBstage1->stage_empty=true;
			 }
			  if(WBstage2->stage_empty==false){
			 	WBstage2->stage_empty=true;
			 }


			if(first_time==false && IFstage1->stage_empty && IFstage2->stage_empty  && IDstage1->stage_empty && IDstage2->stage_empty &&RRstage->stage_empty 
			&& EXstage1->stage_empty &&  EXstage2->stage_empty && MEMstage1->stage_empty && MEMstage2->stage_empty && WBstage1->stage_empty
			&&WBstage2->stage_empty ){
				break;
			}
			first_time=false;
			++clockCycles;

//------------------------------------

			//WB2
			if(MEMstage2->stage_empty==false && WBstage2->stage_empty==true){

				WBstage2->PCcurr= MEMstage2->PCcurr;

				//cout<<"WBstage2 "<< WBstage2->PCcurr<<endl;

				WBstage2->reg_to_update=MEMstage2->reg_to_update;
				WBstage2->value_to_update=MEMstage2->value_to_update;
				
				if(WBstage2->reg_to_update!=""){

			  		//cout<<WBstage->reg_to_update<<" "<<WBstage->value_to_update<<endl;
					//it is writen by the register memory i.e it is updated now
					registers[registerMap[WBstage2->reg_to_update]] =WBstage2->value_to_update;
					reg_to_be_updated[WBstage2->reg_to_update]=0;

				}

				MEMstage2->stage_empty=true;
				WBstage2->stage_empty=false;
			}

			//MEM2 ---used for nothing--but to transfer register to update to WB stage
			if(MEMstage1->stage_empty==false && MEMstage2->stage_empty==true ){

				MEMstage2->PCcurr=MEMstage1->PCcurr;
				MEMstage2->is_branch=MEMstage1->is_branch;
				command = commands[MEMstage2->PCcurr];
				//cout<<"MEMstage2 "<< MEMstage2->PCcurr<<endl;

				// MEMstage2->reg_to_update= MEMstage1->reg_to_update;
				// MEMstage2->value_to_update= MEMstage1->value_to_update;
				
				if(MEMstage2->is_branch==false){

						if (instructions.find(command[0]) == instructions.end())
						{
							handleExit(SYNTAX_ERROR, clockCycles);
							return;
						}

						exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);

						//result will be stored in temporary global register temp


						if (ret != SUCCESS)
						{
							handleExit(ret, clockCycles);
							return;
						}
						++commandCount[MEMstage2->PCcurr];
						PCcurr = PCnext;


						if(command[0]=="lw"){
							MEMstage2->reg_to_update=command[1];
							MEMstage2->value_to_update=temp;
							//UPDATED L5
							reg_to_be_updated[MEMstage2->reg_to_update]=5;
							L5[MEMstage2->reg_to_update]=MEMstage2->value_to_update;
								
						}else{
							//if sw we will update memory in this stage only
							MEMstage2->reg_to_update="";
							MEMstage2->value_to_update=-1;
							
							
						}
				}
				


				MEMstage1->stage_empty=true;
				MEMstage2->stage_empty=false;
				//EXstage2->is_branch=RRstage->is_branch;
				

			}


			//MEM1
			//----SECOND HALF CYCLE(MEM,EX,ID,IF)-----------
			if(EXstage2->stage_empty==false && MEMstage1->stage_empty==true){

				
			
				//MEMstage->is_ALU=EXstage->is_ALU;
				MEMstage1->is_branch=EXstage2->is_branch;
				

				MEMstage1->PCcurr= EXstage2->PCcurr;
				command = commands[MEMstage1->PCcurr];

				//cout<<"MEMstage1 "<<MEMstage1->PCcurr<<endl;

				if(MEMstage1->is_branch==true){
					
					//braching and j-- calculating next pc pcnext
					//MEMstage->PCcurr= EXstage->PCcurr;
						
						if (instructions.find(command[0]) == instructions.end())
						{
							handleExit(SYNTAX_ERROR, clockCycles);
							return;
						}

						exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);

						//result will be stored in temporary global register temp


						if (ret != SUCCESS)
						{
							handleExit(ret, clockCycles);
							return;
						}
						++commandCount[MEMstage1->PCcurr];
						PCcurr = PCnext;
						MEMstage1->reg_to_update="";
						MEMstage1->value_to_update=-1;
							
						EXstage2->stage_empty=true;
						MEMstage1->stage_empty=false;


						//aftert this IF stage is ready to accept nex instructions
						IFstage1->ready_to_take_instruction=true;


				}

				EXstage2->stage_empty=true;
				MEMstage1->stage_empty=false;
			}


			//EX2--used for nothing- but tell it is brach or not
			if(RRstage->stage_empty==false && EXstage2->stage_empty==true &&  RRstage->is_ALU==false){

				RRstage->stage_empty=true;
				EXstage2->PCcurr=RRstage->PCcurr;
				EXstage2->stage_empty=false;
				EXstage2->is_branch=RRstage->is_branch;
				//cout<<"EXstage2 "<< EXstage2->PCcurr<<endl;

			}



//---------------------------
			//ALU BRACH--WB1 AND EX1-------------------------------------
			//-------FIRST HALF CYCLE(WB ONLY)-------

			//WB1
			if(EXstage1->stage_empty==false && WBstage1->stage_empty==true){

				WBstage1->PCcurr= EXstage1->PCcurr;

				//cout<<"WBstage1 "<< WBstage1->PCcurr<<endl;

				WBstage1->reg_to_update=EXstage1->reg_to_update;
				WBstage1->value_to_update=EXstage1->value_to_update;
				
				if(WBstage1->reg_to_update!=""){

			  		//cout<<WBstage->reg_to_update<<" "<<WBstage->value_to_update<<endl;
					//it is writen by the register memory i.e it is updated now
					registers[registerMap[WBstage1->reg_to_update]] =WBstage1->value_to_update;
					reg_to_be_updated[WBstage1->reg_to_update]=0;

				}

				EXstage1->stage_empty=true;
				WBstage1->stage_empty=false;
			}
			
			//EX1
			if(RRstage->stage_empty==false && EXstage1->stage_empty==true &&  RRstage->is_ALU==true){


		
				//if register needed is -to be update instruction will remain in id only
				command = commands[RRstage->PCcurr];
				EXstage1->is_ALU=RRstage->is_ALU;
				//EXstage->is_branch= IDstage->is_branch;
					//reg_to_be_updated[command[1] ]==0 && reg_to_be_updated[command[2] ]==0 
				
				EXstage1->PCcurr= RRstage->PCcurr;
				
				//cout<<"EXstage1 "<<EXstage1->PCcurr<<endl;
				//u have do something in this stage if it is an alu instuction otherwise just don't do anything and move instructions
				//forwaring in stages
				
					
					//if it alu but registered needed are ready to use
					//for addi,sll ,srl only check one register and for other check both registers
						
				if (instructions.find(command[0]) == instructions.end())
				{
					handleExit(SYNTAX_ERROR, clockCycles);
					return;
				}

				exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);

						//result will be stored in an global register temp


				if (ret != SUCCESS)
				{
					handleExit(ret, clockCycles);
					return;
				}
				++commandCount[EXstage1->PCcurr];
				PCcurr = PCnext;

				reg_to_be_updated[command[1]]=1;

				EXstage1->reg_to_update=command[1];
				EXstage1->value_to_update=temp;

				//Updated L4
				reg_to_be_updated[EXstage1->reg_to_update]=4;
				L4[EXstage1->reg_to_update]=temp;

				RRstage->stage_empty=true;
				EXstage1->stage_empty=false;

			}

//-----------------------------------------------------------------------------

			

			//RR
			//this stage just to know which operation is it , it ALU or not ALU and also set reg_to_be_updated
			if(IDstage2->stage_empty==false && RRstage->stage_empty==true && IDstage2->reading_done==true ){
			

			    
				IDstage2->stage_empty=true;
				RRstage->PCcurr= IDstage2->PCcurr;
				
				
				command = commands[RRstage->PCcurr];
				//IDstage->reading_done=true;
				//cout<<"RRstage "<< RRstage->PCcurr<<endl;
					
			 	if (instructions.find(command[0]) == instructions.end())
			 	{
			 		handleExit(SYNTAX_ERROR, clockCycles);
			 		return;
			 	}

				if(command[0]=="add" || command[0]=="sub"|| command[0]=="mul" || command[0]=="slt"|| command[0]=="addi" ||command[0]=="sll" || command[0]=="srl"){
					//ALU instuction at EX stage
					RRstage->is_ALU=true;
					reg_to_be_updated[command[1]]=1;
					//IDstage->is_branch=false;
					//IDstage->reading_done=false;
					// if((command[0]=="addi"||command[0]=="sll"||command[0]=="srl") && reg_to_be_updated[command[2] ]==0 || command[0]!="addi"&& command[0]!="sll" && command[0]!="srl"&&reg_to_be_updated[command[2]]==0 && reg_to_be_updated[command[3]]==0  ){

					// 	IDstage->reading_done=true;	
					// }
			
					
				}else if(command[0]=="lw"|| command[0]=="sw"){
					//load/store at MEM stage
					// templeft = command[2].find('(');
					// tempreg = command[2].substr(templeft + 1);
				     // tempreg.pop_back();
					//  cout<<tempreg<<endl;

					 if(command[0]=="lw"){
						
					// 	IDstage->reading_done=false;
						
					// 	if(reg_to_be_updated[tempreg]==0){
					// 		IDstage->reading_done=true;
					// 		//jugaad to handle lw $1 10($1)
					 		reg_to_be_updated[command[1]]=1;
					}

					// }else{
					// 	IDstage->reading_done=false;
						
					// 	if(reg_to_be_updated[tempreg]==0 && reg_to_be_updated[command[1]]==0){
					// 		IDstage->reading_done=true;
					// 	}
					// }
					RRstage->is_ALU=false;
					RRstage->is_branch=false;
					
				}else if(command[0]=="beq"|| command[0]=="bne" ){
					//brach and jump instruction
					//IF stage will not be taking new instructions untill PCnext is calculated in MEME stage
					RRstage->is_branch=true;
					//IFstage->ready_to_take_instruction=false;

					//breach is our ALU does not matter much
					RRstage->is_ALU=false;
					//IDstage->reading_done=false;

					// if((command[0]=="beq"|| command[0]=="bne")&& reg_to_be_updated[command[1]]==0 && reg_to_be_updated[command[2]]==0 ){
					// 	IDstage->reading_done=true;
					// }

					// if(command[0]=="j"){
					// 	IDstage->reading_done=true;
					// }

				//jump instruciotn after the ID2 STAGE
				}else if(command[0]=="j"){
					
					RRstage->is_branch=false;
					//IFstage->ready_to_take_instruction=false;

					
					//breach is our ALU does not matter much
					RRstage->is_ALU=false;

					 //jump instruction in EXstage i.e after ID STAGE
						if (instructions.find(command[0]) == instructions.end())
						{
							handleExit(SYNTAX_ERROR, clockCycles);
							return;
						}

						exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);

						//result will be stored in temporary global register temp


						if (ret != SUCCESS)
						{
							handleExit(ret, clockCycles);
							return;
						}
						++commandCount[RRstage->PCcurr];
						PCcurr = PCnext;
							//MEMstage->reg_to_update="";
							//MEMstage->value_to_update=-1;
						//aftert this IF stage is ready to accept nex instructions
						IFstage1->ready_to_take_instruction=true;
				}

				//  for(auto x:command){
				//  	cout<<x<<endl;
				//  }

				
				RRstage->stage_empty=false;
			}



			//ID2--check where we have all component available for reading
			if(IDstage1->stage_empty==false && IDstage2->stage_empty==true || IDstage2->stage_empty==false){
			

			    if(IDstage1->stage_empty==false && IDstage2->stage_empty==true){
					IDstage1->stage_empty=true;
					IDstage2->PCcurr= IDstage1->PCcurr;
				}
				
				command = commands[IDstage2->PCcurr];
				IDstage2->reading_done=true;
				//cout<<"IDstage2 "<< IDstage2->PCcurr<<endl;
					
			 	if (instructions.find(command[0]) == instructions.end())
			 	{
			 		handleExit(SYNTAX_ERROR, clockCycles);
			 		return;
			 	}

				if(command[0]=="add" || command[0]=="sub"|| command[0]=="mul" || command[0]=="slt"
				|| command[0]=="addi" ||command[0]=="sll" || command[0]=="srl" 
				|| command[0]=="and"   || command[0]=="or" || command[0]=="xor"){
					//ALU instuction at EX stage
					//IDstage->is_ALU=true;
					//reg_to_be_updated[command[1]]=1;
					//IDstage->is_branch=false;
					IDstage2->reading_done=false;
					if((command[0]=="addi"||command[0]=="sll"||command[0]=="srl") && reg_to_be_updated[command[2] ]!=1 || command[0]!="addi"&& command[0]!="sll" && command[0]!="srl"&&reg_to_be_updated[command[2]]!=1 && reg_to_be_updated[command[3]]!=1  ){

						IDstage2->reading_done=true;	
					}
			
					
				}else if(command[0]=="lw"|| command[0]=="sw"){
					//load/store at MEM stage
					templeft = command[2].find('(');
					tempreg = command[2].substr(templeft + 1);
				     tempreg.pop_back();
					// cout<<tempreg<<endl;

					if(command[0]=="lw"){
						
						IDstage2->reading_done=false;
						
						if(reg_to_be_updated[tempreg]!=1){
							IDstage2->reading_done=true;
							//jugaad to handle lw $1 10($1)
							reg_to_be_updated[command[1]]=1;
						}

					}else{
						IDstage2->reading_done=false;
						
						if(reg_to_be_updated[tempreg]!=1 && reg_to_be_updated[command[1]]!=1){
							IDstage2->reading_done=true;
						}
					}
					//IDstage->is_ALU=false;
					//IDstage->is_branch=false;
					
				}else if(command[0]=="j" || command[0]=="beq"|| command[0]=="bne" ){
					//brach and jump instruction
					//IF stage will not be taking new instructions untill PCnext is calculated in MEME stage
					//IDstage->is_branch=true;
					//IFstage->ready_to_take_instruction=false;
					//IDstage->is_ALU=false;
					IDstage2->reading_done=false;

					if((command[0]=="beq"|| command[0]=="bne")&& reg_to_be_updated[command[1]]!=1 && reg_to_be_updated[command[2]]!=1 ){
						IDstage2->reading_done=true;
					}

					if(command[0]=="j"){
						IDstage2->reading_done=true;
					}

				}

				//  for(auto x:command){
				//  	cout<<x<<endl;
				//  }

				
				IDstage2->stage_empty=false;
			}



			//ID1 -- used for nothing
			if(IFstage2->stage_empty==false && IDstage1->stage_empty==true ){
			

			    
				IFstage2->stage_empty=true;
				IDstage1->PCcurr= IFstage2->PCcurr;
				//cout<<"IDstage1 "<< IDstage1->PCcurr<<endl;

				IDstage1->stage_empty=false;

				
				/*
				command = commands[IDstage->PCcurr];
				IDstage->reading_done=true;
				cout<<"IDstage "<< IDstage->PCcurr<<endl;
					
			 	if (instructions.find(command[0]) == instructions.end())
			 	{
			 		handleExit(SYNTAX_ERROR, clockCycles);
			 		return;
			 	}

				if(command[0]=="add" || command[0]=="sub"|| command[0]=="mul" || command[0]=="slt"|| command[0]=="addi" ||command[0]=="sll" || command[0]=="srl"){
					//ALU instuction at EX stage
					IDstage->is_ALU=true;
					//reg_to_be_updated[command[1]]=1;
					IDstage->is_branch=false;
					IDstage->reading_done=false;
					if((command[0]=="addi"||command[0]=="sll"||command[0]=="srl") && reg_to_be_updated[command[2] ]==0 || command[0]!="addi"&& command[0]!="sll" && command[0]!="srl"&&reg_to_be_updated[command[2]]==0 && reg_to_be_updated[command[3]]==0  ){

						IDstage->reading_done=true;	
					}
			
					
				}else if(command[0]=="lw"|| command[0]=="sw"){
					//load/store at MEM stage
					templeft = command[2].find('(');
					tempreg = command[2].substr(templeft + 1);
				     tempreg.pop_back();
					 cout<<tempreg<<endl;

					if(command[0]=="lw"){
						
						IDstage->reading_done=false;
						
						if(reg_to_be_updated[tempreg]==0){
							IDstage->reading_done=true;
							//jugaad to handle lw $1 10($1)
							reg_to_be_updated[command[1]]=1;
						}

					}else{
						IDstage->reading_done=false;
						
						if(reg_to_be_updated[tempreg]==0 && reg_to_be_updated[command[1]]==0){
							IDstage->reading_done=true;
						}
					}
					IDstage->is_ALU=false;
					IDstage->is_branch=false;
					
				}else if(command[0]=="j" || command[0]=="beq"|| command[0]=="bne" ){
					//brach and jump instruction
					//IF stage will not be taking new instructions untill PCnext is calculated in MEME stage
					IDstage->is_branch=true;
					IFstage->ready_to_take_instruction=false;
					IDstage->is_ALU=false;
					IDstage->reading_done=false;

					if((command[0]=="beq"|| command[0]=="bne")&& reg_to_be_updated[command[1]]==0 && reg_to_be_updated[command[2]]==0 ){
						IDstage->reading_done=true;
					}

					if(command[0]=="j"){
						IDstage->reading_done=true;
					}

				}

				//  for(auto x:command){
				//  	cout<<x<<endl;
				//  }

				
				IDstage->stage_empty=false;
			
				*/
			}

			
			//IF2--used to check where it is branch/jump instruction --so that we can stop IFstag1 to take more instructions
			if(IFstage1->stage_empty==false && IFstage2->stage_empty==true){
				
				//while j and brach instuction not compute next PC are there IF stage will not take new instruction
				//j and branch instruction will compure next PC in MEM stage

				//cout<<PCnext<<endl;

				IFstage1->stage_empty=true;
				IFstage2->PCcurr= IFstage1->PCcurr;
				//cout<<"IFstage2 "<< IFstage2->PCcurr<<endl;

				IFstage2->stage_empty=false;

				command = commands[IFstage2->PCcurr];
				
					
			 	if (instructions.find(command[0]) == instructions.end())
			 	{
			 		handleExit(SYNTAX_ERROR, clockCycles);
			 		return;
			 	}

				if(command[0]=="j" || command[0]=="beq"|| command[0]=="bne" ){
					//brach and jump instruction
					//IF stage will not be taking new instructions untill PCnext is calculated in MEME stage
					//IDstage->is_branch=true;
					IFstage1->ready_to_take_instruction=false;
					//IDstage->is_ALU=false;
					//IDstage2->reading_done=false;

					// if((command[0]=="beq"|| command[0]=="bne")&& reg_to_be_updated[command[1]]==0 && reg_to_be_updated[command[2]]==0 ){
					// 	IDstage2->reading_done=true;
					// }

					// if(command[0]=="j"){
					// 	IDstage2->reading_done=true;
					// }

				}

			
			}

			//IF1
			if(IFstage1->stage_empty==true){
				
				//while j and brach instuction not compute next PC are there IF stage will not take new instruction
				//j and branch instruction will compure next PC in MEM stage

				//cout<<PCnext<<endl;

				if(IFstage1->ready_to_take_instruction==true && PCnext<commands.size()){

				   
					IFstage1->PCcurr=PCnext;
					// cout<<"IFstage1 "<< IFstage1->PCcurr<<endl;
					PCnext=PCnext+1;
					IFstage1->stage_empty=false;
					
				}else{
					IFstage1->stage_empty=true;
				}
				
			}




			//////////////////////////------------------////////////////////////////////////////

			//AT THIS POINT WE WILL CHANGE OUR LATCH L4,L5,L6
			L6=L5;
			L5.clear();
			L4.clear();

			for(auto reg : reg_to_be_updated){
				if(reg_to_be_updated[reg.first]==5){
					reg_to_be_updated[reg.first]=6;
				}
			}

			//cout<<IFstage->PCcurr<<" "<<IDstage->PCcurr<<" "<<EXstage->PCcurr<<" "<<MEMstage->PCcurr<<" "<<WBstage->PCcurr<<endl;
				printRegisters(clockCycles);
			if(if_address__value_change==true){
				cout<<that_address_changed<<" "<<that_vaue_changed<<endl;
			}else{
				cout<<0<<endl;
			}



			//cout<<" "<<endl;
		}

		
		handleExit(SUCCESS, clockCycles);
	}




	// print the register data in hexadecimal
	void printRegisters(int clockCycle)
	{
		//std::cout << "Cycle number: " << clockCycle << '\n'
		//		  << std::hex;
		for (int i = 0; i < 32; ++i)
			std::cout << registers[i] << ' ';
		std::cout << std::dec << '\n';
	
	
	}





};

#endif