/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 * 
 */

#ifndef __ID_HPP__
#define __ID_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>

struct ID
{
	
	int PCcurr=-1;

	int input=0;
	int output=0;
	int control_signal=0;
	bool is_ALU;
	bool stage_empty;
	bool is_branch;
	bool reading_done;
	bool is_jump;

	// constructor to initialise the instruction set
	ID()
	{
		this->PCcurr=-1;
		this->input=-1;
		this->output=-1;
		this->control_signal=-1;
		this->is_ALU=false;
		this->stage_empty=true;
		this->is_branch=false;
		this->reading_done=false;
		this->is_jump=false;
	}


};

#endif