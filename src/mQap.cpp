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

	this->explored = false;


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

Solution::Solution(Matrix& dist, Matrix& flow1, Matrix& flow2, std::vector<int> aux){
	this->dist = dist;
	this->flow1 = flow1;
	this->flow2 = flow2;
	this->solution = aux;

	this->explored = false;

	this->costs.resize(n_obj);
	
	int cost1 = 0;
	int cost2 = 0;

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
	this->index = b.index;
}

//Testando se this domina b
bool Solution::operator>(Solution& b){
	/*int greater_qtd = 0;
	int smaller_qtd = 0;
	
	for(int i(0); i < n_obj; i++){
		if(this->costs[i] < b.costs[i]){
			smaller_qtd++;
		}

		if(this->costs[i] > b.costs[i]){
			greater_qtd++;
		}
	}

	if(greater_qtd > 0 and smaller_qtd == 0){
		return true;
	}

	return false;*/
	return (this->costs[0] > b.costs[0] and this->costs[1] > b.costs[1]) or
			(this->costs[0] >= b.costs[0] and this->costs[1] > b.costs[1]) or
			(this->costs[0] > b.costs[0] and this->costs[1] >= b.costs[1]); 
}

bool Solution::is_non_dominated(Solution b){
	bool non_dominated = false;

	if((this->costs[0] < b.costs[0] and this->costs[1] > b.costs[1]) or
		(this->costs[0] > b.costs[0] and this->costs[1] < b.costs[1]) or
		(this->costs[0] == b.costs[0] and this->costs[1] == b.costs[1])){
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
	n_sol = 5000;

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

bool greater(Solution& a, Solution& b){
	return a.costs[0] > b.costs[0];
}

std::vector<Solution> MQap::generate_non_dominated_solutions(){
	std::vector<Solution> solutions;
	for(int i(0); i < n_sol; i++){
		Solution sol(dist, flow1, flow2);
		sol.index = i;
		solutions.push_back(sol);
	}

	//Ordena-se com base na primeira função objetivo
	sort(solutions.begin(), solutions.end(), greater);
	//Insere-se o primeiro elemento no conjunto não dominado
	std::vector<Solution> s1;
	s1.push_back(solutions[0]);


	for(unsigned i(1); i < solutions.size(); i++){
		//Verifica-se se o elemento do conjunto não dominado é dominado por alguma das soluções
		for(unsigned j(0); j < s1.size(); j++){	
			if(solutions[i] > s1[j]){
				s1.erase(s1.begin() + j);
				break;
			}
		}

		//Testa-se se a solução é não dominada com relação a todos do conjunto não dominado
		bool is_non_dominated = true;
		for(unsigned j(0); j < s1.size(); j++){
			if(not solutions[i].is_non_dominated(s1[j])){
				is_non_dominated = false;
				break;
			}
		}

		if(is_non_dominated){
			s1.push_back(solutions[i]);
		}

		if(s1.size() == 0){
			s1.push_back(solutions[i]);
		}

		//std::cout << solutions[i].costs[0] << " " << solutions[i].costs[1] << "\n";
	}


	return s1;
}

long ohvc(Solution& s1, Solution& s2){
	return (s1.costs[0] - s2.costs[0])*(s2.costs[1] - s1.costs[1]);
}

void Solution::swap_solution(int& a, int& b){
	int temp = a;
	a = b;
	b = temp;

	int cost1 = 0;
	int cost2 = 0;

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			cost1 += this->flow1[this->solution[i]][this->solution[j]] * this->dist[i][j];
			cost2 += this->flow2[this->solution[i]][this->solution[j]] * this->dist[i][j];
		}
	}
	
	this->costs[0] = cost1;
	this->costs[1] = cost2;
}


void MQap::improvement(Solution& current){

	std::vector<int> aux = current.solution;
	Solution neighbor(current.dist, current.flow1, current.flow2);

	/*for(unsigned i(0); i < current.solution.size(); i++){
		std::cout << current.solution[i] << " ";
	}
	std::cout << "\n\n";
	
	std::cout << current.costs[0] << " " << current.costs[1] << "\n";
	*/

	//neighbor.swap_solution(neighbor.solution[0], neighbor.solution[1]);
	unsigned arch_size = archive.size();
	bool exit = false;
	for(int i(0); i < n - 1; i++){
		for(int j(i + 1); j < n; ++j){
			neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);

			//std::cout << neighbor.costs[0] << " " << neighbor.costs[1] << "\n";		
			//std::cout << current.costs[0] << " " << current.costs[1] << "\n\n";

			/*for(int k(0); k < n; k++){
				std::cout << neighbor.solution[k] << " ";
			}
			std::cout << "\n";
			*/

			if(neighbor > current){
				std::cout << "******************************push1*********************************\n";
				archive.push_back(neighbor);
				current.explored = true;
				arch_size++;
				exit = true;
			}

			neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
			
			if(exit){
				return;
			}
		}	
	}

	/*for(unsigned i(0); i < current.solution.size(); i++){
		std::cout << current.solution[i] << " ";
	}
	std::cout << "\n\n";
	
	std::cout << current.costs[0] << " " << current.costs[1] << "\n";
	*/

	neighbor = current;
	arch_size = archive.size();
	for(int i(0); i < n - 1; i++){
		for(int j(i + 1); j < n; ++j){
			neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);

			bool non_dominated = true;
			for(unsigned k(0); k < arch_size; k++){
				if(archive[k] > neighbor){
					non_dominated = false;
					break;
				}
			}

			if(non_dominated){
				//std::cout << "\n" << neighbor.costs[0] << " " << neighbor.costs[1] << "\n\n";
				for(unsigned k(0); k < arch_size; k++){
					//std::cout << archive[k].costs[0] << " " << archive[k].costs[1] << "\n";
					if(neighbor > archive[k]){
						archive.erase(archive.begin() + k);
						k--;
						arch_size--;
					}
				}
				std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%push2%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
				archive.push_back(neighbor);
				arch_size++;
			}

			neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
		}
	}

	current.explored = true;
}

void MQap::anytime_pareto_local_search(){
	//Gerando soluções não dominadas mutuamente.
	archive0.reserve(10000);
	archive.reserve(10000);

	archive0 = generate_non_dominated_solutions();

	archive = archive0;

	do{
		unsigned arch_size = archive0.size();

		//Vetor para armazenar os valores de ohi de cada solução.
		std::vector<long> ohi;
		ohi.resize(arch_size);


		for(unsigned i(0); i < arch_size; i++){
			int inf_index = -1;
			int sup_index = -1;
			//Encontrar a solução superior mais próxima a solução atual
			for(unsigned j(0); j < arch_size; j++){
				if(sup_index == -1 and archive0[j].costs[1] > archive0[i].costs[1]){
					sup_index = j;
				}
				
				if(sup_index != -1 and archive0[sup_index].costs[1] > archive0[j].costs[1] and 
															archive0[j].costs[1] > archive0[i].costs[1]){
					sup_index = j;
				}		
			}


			//Encontrar a solução inferior mais próxima a solução atual
			for(unsigned j(0); j < arch_size; j++){
				if(inf_index == -1 and archive0[j].costs[1] < archive0[i].costs[1]){
					inf_index = j;
				}

				if(inf_index != -1 and archive0[inf_index].costs[1] < archive0[j].costs[1] and 
															archive0[j].costs[1] < archive0[i].costs[1]){
					inf_index = j;
				}
				
			}

			if(arch_size > 1){
				if(inf_index == -1){
					//std::cout << "*****1*****\n";
					ohi[i] = 2*ohvc(archive0[sup_index], archive0[i]);
				}else if(sup_index == -1){
					//std::cout << "*****2*****\n";
					ohi[i] = 2*ohvc(archive0[i], archive0[inf_index]);
				}else{
					//std::cout << "*****3*****\n";
					ohi[i] = ohvc(archive0[sup_index], archive0[i]) + ohvc(archive0[i], archive0[inf_index]);
				}
			}
		}
		
		//Encontrando indice da solução com maior ohi
		int max_index = 0;
		if(arch_size > 1){
			for(unsigned i(0); i < arch_size; i++){ 
				if(ohi[i] > ohi[max_index]){
					max_index = i;
				}
			}
		}else{
			max_index = 0;
		}

		int search_index = archive0[max_index].index;
		int current_index = -1;

		for(unsigned i(0); i < arch_size; i++){
			if(archive[i].index == search_index){
				current_index = i;
			}
		}

		Solution neighbor(archive[current_index].dist, archive[current_index].flow1, archive[current_index].flow2,
								 archive[current_index].solution);
		

		for(int i(0); i < n - 1; i++){
			for(int j(i + 1); j < n; ++j){
				neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);

				neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
			}
		}








		/*std::vector<Solution> temp;
		for(unsigned i(0); i < archive.size(); i++){
			if(archive[i].explored == false){
				temp.push_back(archive[i]);
			}
		}

		archive0 = temp;
		*/
		//std::cout << archive.size() << "fg\n";

	}while(archive0.size() != 0);


}