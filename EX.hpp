/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 * 
 */

#ifndef __EX_HPP__
#define __EX_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>


struct EX
{
	
	int PCcurr=-1;

	int input=0;
	int output=0;
	int control_signal=0;
	bool is_ALU=false;
	std::string reg_to_update;
	int value_to_update;
	bool stage_empty;
	bool is_branch;
	bool is_jump;
	// constructor to initialise the instruction set
	EX()
	{
		this->PCcurr=-1;
		this->input=-1;
		this->output=-1;
		this->control_signal=-1;
		this->is_ALU=false;
		this->reg_to_update="";
		this->value_to_update=-1;
		this->stage_empty=true;
		this->is_branch=false;
		this->is_jump=false;
	}


};

#endif