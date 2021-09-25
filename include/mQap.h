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
	Solution(Matrix& dist, Matrix& flow1, Matrix& flow2, std::vector<int> aux);
	Solution();

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	
	//Testa se as soluções tem os mesmo valores de custos
	bool operator==(Solution& b);
	//Realiza atribuição de soluções
	void operator=(Solution b);
	//Verifica se b é dominada pela solução atual
	bool operator<(Solution& b);
	//Testa se as soluções são mutuamente não dominadas
    bool is_non_dominated(Solution b);
    //Faz uma troca no vetor solução
    void swap_solution(int& a, int& b);

	//Alocação escolhida
	std::vector<int> solution;
	//Vetor de custos (resposta das funções objetivos)
	std::vector<long> costs;
	//Identifica como explorada ou não
	bool explored = false;
	//Índice da solucao
	int index;
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

		void read_instance(std::string filename, int inst_size);
		int get_inst_size();
		int get_obj_qtd();
		Matrix& get_dist_matrix();
		Matrix& get_flow1_matrix();
		Matrix& get_flow2_matrix();

		void improvement(Solution& current);
		void apply_first_exploration();
		void apply_best_exploration();
		void anytime_pareto_local_search();
		std::vector<Solution> generate_non_dominated_solutions();
		std::vector<Solution> path_relinking();
		bool is_dominated_by_archive_post_processing(Solution& solution, int arch_size);
		bool has_equal_on_archive(Solution& solution, int arch_size);

	private:
		Matrix dist;
		Matrix flow1;
		Matrix flow2;
		std::vector<Solution> archive;
		std::vector<Solution> archive0;
		Solution best_solution;
};

#endif