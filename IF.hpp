/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 * 
 */

#ifndef __IF_HPP__
#define __IF_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>

struct IF
{
	
	int PCcurr=-1;

	int input=0;
	int output=0;
	int control_signal=0;
	bool ready_to_take_instruction=0;
	bool stage_empty;

	// constructor to initialise the instruction set
	IF()
	{
		this->PCcurr=-1;
		this->input=-1;
		this->output=-1;
		this->control_signal=-1;
		this->ready_to_take_instruction=true;
		this->stage_empty=true;
		
	}


};

#endif