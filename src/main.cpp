#include <iostream>
#include "Pipeline.h"
#include <fstream>
#include <string>

// TODO: Correctness: Handle user errors

int main(int argc, char** argv)
{
	std::string filename = ROOT "/meta/debug.txt";
	std::ifstream fileInput(filename);
	if (fileInput.bad())
	{
		printf("Couldn't open file");
		return -1;
	}

	Pipeline p;
	p.build(fileInput,filename);
}
