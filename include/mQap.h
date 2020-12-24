#ifndef MQAP_H
#define MQAP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>    // std::shuffle

using Matrix = std::vector<std::vector<int>>;

struct Solution{

	Solution(Matrix& dist, Matrix& flow1, Matrix& flow2);
	Solution();

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	
	void operator=(Solution b);
	//Verifica se b é dominada pela solução atual
	bool operator>(Solution& b);
	//Testa se as soluções são mutuamente não dominadas
    bool is_non_dominated(Solution b);
    //Faz uma troca no vetor solução
    void swap_solution(int& a, int& b);

	//Alocação escolhida
	std::vector<int> solution;
	//Vetor de custos (resposta das funções objetivos)
	std::vector<int> costs;
	//Identifica como explorada ou não
	bool explored;
	//Matrix de distancia
	Matrix dist;
	//Matrix de fluxo 1
	Matrix flow1;
	//Matrix de fluxo 2
	Matrix flow2;
};


class MQap{
	public:
		MQap();

		void read_instance();
		int get_inst_size();
		int get_obj_qtd();
		Matrix& get_dist_matrix();
		Matrix& get_flow1_matrix();
		Matrix& get_flow2_matrix();

		void anytime_pareto_local_search();
		std::vector<Solution> generate_non_dominated_solutions();

	private:
		Matrix dist;
		Matrix flow1;
		Matrix flow2;
		std::vector<Solution> archive;
		Solution best_solution;
};

#endif