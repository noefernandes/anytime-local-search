#include <iostream>
#include <chrono>
#include "mQap.h"

int main(int argc, char *argv[]){
	//Realizando leitura das matrizes de distancia e fluxos, e atribindo as variaveis. 
	std::string filename = argv[1];
	int inst_size = atoi(argv[2]);

	MQap mQap;
	mQap.read_instance(filename, inst_size);
	

	std::vector<double> time_total;
	std::clock_t start, end;

	std::vector<Solution> pf;
	for(int i = 0; i < 30; i++){
		start = clock();
		pf = mQap.path_relinking();
		end = clock();
		time_total.push_back(double(end - start) / CLOCKS_PER_SEC);
	}


	for(int i = 0; i < 30; i++){
		std::cout << time_total[i] << std::endl;
	}

	//mQap.path_relinking();
	
}