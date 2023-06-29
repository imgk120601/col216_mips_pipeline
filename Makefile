compile: run_5stage run_5stage_bypass run_79stage run_79stage_bypass


run_5stage: 5stage.cpp MIPS_Processor1.hpp
	g++ 5stage.cpp MIPS_Processor1.hpp -o run_5stage

run_5stage_bypass: 5stage_bypass.cpp MIPS_Processor2.hpp
	g++ 5stage_bypass.cpp MIPS_Processor2.hpp -o run_5stage_bypass

run_79stage: 79stage.cpp MIPS_Processor3.hpp
	g++ 79stage.cpp MIPS_Processor3.hpp -o run_79stage

run_79stage_bypass: 79stage_bypass.cpp MIPS_Processor4.hpp
	g++ 79stage_bypass.cpp MIPS_Processor4.hpp -o run_79stage_bypass

clean:
	rm run_5stage
	rm run_5stage_bypass
	rm run_79stage
	rm run_79stage_bypass
