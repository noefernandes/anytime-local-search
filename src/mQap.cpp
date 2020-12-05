#include "mQap.h"

//Tamanho da instancia
int n;
//Numero de objetivos
int n_obj;
//Numero de solucoes inicialmente no arquivo
int n_sol;

Solution::Solution(){}

Solution::Solution(Matrix& dist, Matrix& flow1, Matrix& flow2){
	this->dist = dist;
	this->flow1 = flow1;
	this->flow2 = flow2; 


	for(int i(0); i < n; i++){
		this->solution.push_back(i);
	}

	this->costs.resize(n_obj);
	int cost1 = 0;
	int cost2 = 0;

	shuffle(this->solution.begin(), this->solution.end(), std::default_random_engine(seed));

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			cost1 += this->flow1[this->solution[i]][this->solution[j]] * this->dist[i][j];
			cost2 += this->flow2[this->solution[i]][this->solution[j]] * this->dist[i][j];
		}
	}
	
	this->costs[0] = cost1;
	this->costs[1] = cost2;
}

void Solution::operator=(Solution b){
	this->solution = b.solution;
	this->costs = b.costs;
}

//Testando se this domina b
bool Solution::operator<(Solution b){
	int greater_qtd = 0;
	int smaller_qtd = 0;
	
	for(int i(0); i < n_obj; i++){
		if(this->costs[i] < b.costs[i]){
			smaller_qtd++;
		}

		if(this->costs[i] > b.costs[i]){
			greater_qtd++;
		}
	}

	if(smaller_qtd > 0 and greater_qtd == 0){
		return true;
	}

	return false;
}

bool Solution::is_non_dominated(Solution b){
	bool non_dominated = false;

	if((this->costs[0] < b.costs[0] and this->costs[0] > b.costs[0]) or
		(this->costs[0] > b.costs[0] and this->costs[0] < b.costs[0]) or
		(this->costs[0] == b.costs[0] and this->costs[0] == b.costs[0])){
		non_dominated = true;
	}

	return non_dominated;
}

/***********************************************************************************************/

MQap::MQap(){}

int MQap::get_inst_size(){ return n; }
int MQap::get_obj_qtd(){ return n; }
Matrix& MQap::get_dist_matrix(){ return dist; }
Matrix& MQap::get_flow1_matrix(){ return flow1; }
Matrix& MQap::get_flow2_matrix(){ return flow2; }

void MQap::read_instance(){
	n = 30;
	n_obj = 2;
	n_sol = 10000;

	dist.resize(n);
	flow1.resize(n);
	flow2.resize(n);

	std::ifstream file("data/input.txt");
	std::string line;
	
  	if(file.is_open()){
    	for(int i = 0; i < n; ++i){
			dist[i].resize(n);
			flow1[i].resize(n);
			flow2[i].resize(n);
		}

		for(int i = 0; i < n; ++i){
			for(int j = 0; j < n; ++j){
				file >> dist[i][j];
			}
		}

		for(int i = 0; i < n; ++i){
			for(int j = 0; j < n; ++j){
				file >> flow1[i][j];
			}
		}

		for(int i = 0; i < n; ++i){
			for(int j = 0; j < n; ++j){
				file >> flow2[i][j];
			}
		}	
  	}
}

std::vector<Solution> MQap::generate_non_dominated_solutions(){
	std::vector<Solution> solutions;
	for(int i(0); i < n_sol; i++){
		solutions.push_back(Solution(dist, flow1, flow2));
	}

	for(int i(0); i < n_sol; i++){
		for(unsigned j(0); j < solutions[i].solution.size(); j++){
			std::cout << solutions[i].solution[j] << " ";
		}
		std::cout << "\n\n";
	}

	/*for(unsigned i(0); i < solutions.size() - 1; i++){
		for(unsigned j(i); j < solutions.size(); j++){
			if(not solutions[i].is_non_dominated(solutions[j])){
				//std::cout << "entra\n";
				if(solutions[i] < solutions[j]){
					solutions.erase(solutions.begin() + j);
				}else{
					solutions.erase(solutions.begin() + i);
				}
			}
		}
	}*/

	std::cout << solutions.size();

	return solutions;
}

void MQap::anytime_pareto_local_search(){
	archive = generate_non_dominated_solutions();
}