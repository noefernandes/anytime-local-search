#include <iostream>
#include "mQap.h"

int main(int argc, char *argv[]){
	//Realizando leitura das matrizes de distancia e fluxos, e atribindo as variaveis. 
	MQap mQap;
	mQap.read_instance();

	//Solution s1(mQap.get_dist_matrix(), mQap.get_flow1_matrix(), mQap.get_flow2_matrix());
	//Solution s2(mQap.get_dist_matrix(), mQap.get_flow1_matrix(), mQap.get_flow2_matrix());
	mQap.anytime_pareto_local_search();

	
}