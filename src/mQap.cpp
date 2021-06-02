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
	this->dist = b.dist;
	this->flow1 = b.flow1;
	this->flow2 = b.flow2;
	this->explored = b.explored;
}

//Testando se this domina b
bool Solution::operator<(Solution& b){
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
	return (this->costs[0] < b.costs[0] and this->costs[1] < b.costs[1]) or
			(this->costs[0] <= b.costs[0] and this->costs[1] < b.costs[1]) or
			(this->costs[0] < b.costs[0] and this->costs[1] <= b.costs[1]); 
}

bool Solution::operator==(Solution& b){
	return this->costs[0] == b.costs[0] and this->costs[1] == b.costs[1];
}

/*bool Solution::is_non_dominated(Solution b){
	bool non_dominated = false;

	if((this->costs[0] < b.costs[0] and this->costs[1] > b.costs[1]) or
		(this->costs[0] > b.costs[0] and this->costs[1] < b.costs[1]) or
		(this->costs[0] == b.costs[0] and this->costs[1] == b.costs[1])){
		non_dominated = true;
	}

	return non_dominated;
}
*/

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
		solutions.push_back(sol);;
	}

	//Ordena-se com base na primeira função objetivo
	sort(solutions.begin(), solutions.end(), greater);
	//Insere-se o primeiro elemento no conjunto não dominado
	std::vector<Solution> s1;
	s1.push_back(solutions[0]);

	/*for(unsigned i(0); i < solutions.size(); i++){
		std::cout << "\n" << solutions[i].costs[0] << " " << solutions[i].costs[1] << "\n";
	}

	std::cout << "----------------------------------\n";
	*/

	for(unsigned i(1); i < solutions.size(); i++){
		//Verifica-se se o elemento do conjunto não dominado é dominado por alguma das soluções
		for(unsigned j(0); j < s1.size(); j++){	
			if(solutions[i] < s1[j] or solutions[i] == s1[j]){
				s1.erase(s1.begin() + j);
				//std::cout << "op 1\n";
				j--;
			}
		}

		//Testa-se se a solução é não dominada com relação a todos do conjunto não dominado
		bool is_non_dominated = true;
		for(unsigned j(0); j < s1.size(); j++){
			if(s1[j] < solutions[i]){
				is_non_dominated = false;
				//std::cout << "op 2\n";
				break;
			}
		}

		if(is_non_dominated){
			s1.push_back(solutions[i]);
			//std::cout << "op 2 add\n";
		}

		if(s1.size() == 0){
			s1.push_back(solutions[i]);
			//std::cout << "op 3\n";
		}

		//std::cout << solutions[i].costs[0] << " " << solutions[i].costs[1] << "\n";
	}


	//std::cout << "---------------------------\n";
	for(unsigned i(0); i < s1.size(); i++){
		//std::cout << "\n" << s1[i].costs[0] << " " << s1[i].costs[1] << "\n";
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

void MQap::apply_first_exploration(){

	unsigned arch_size = archive0.size();
	do{
		//archive0 = archive;
		arch_size = archive0.size();
		//archive0 = archive;

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
					ohi[i] = 2*ohvc(archive0[sup_index], archive0[i]);
				}else if(sup_index == -1){
					ohi[i] = 2*ohvc(archive0[i], archive0[inf_index]);
				}else{
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
				


		Solution current;
		current = archive0[max_index];
		Solution neighbor;
		neighbor = current;


		//Testa se encontrou uma solucao que domina a atual, senao aceita-se nao dominados
		bool first = false;
		for(int i(0); i < n - 1; i++){
			for(int j(i + 1); j < n; ++j){
				neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
				arch_size = archive.size();
				for(unsigned k(0); k < arch_size; k++){
					if(neighbor < archive[k]){
						for(unsigned l(0); l < arch_size; l++){
							if(neighbor < archive[l]){
								archive.erase(archive.begin() + l);
								l--;
								arch_size--;
							}
						}

						first = true;
						Solution inserted;
						inserted = neighbor;
						if(arch_size > 0)
							inserted.index = archive[arch_size - 1].index + 1;
						else
							inserted.index = 0;
						archive.push_back(inserted);
						arch_size++;		
						break;
					}
				}

				neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
			}
		}


		if(not first){
			for(int i(0); i < n - 1; i++){
				for(int j(i + 1); j < n; ++j){
					neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
					//Se alguma solução no arquivo domina o vizinho atual, não inseri-lo no arquivo.
					bool non_dominated = true;

					arch_size = archive.size();
					for(unsigned k(0); k < arch_size; k++){
						if(archive[k] < neighbor or archive[k] == neighbor){
							non_dominated = false;
							break;
						}
					}

					//Se a solução é não dominada, retirar os dominados por ela.
					if(non_dominated){
						for(unsigned k(0); k < arch_size; k++){
							if(neighbor < archive[k]){
								//std::cout << archive[k].index << " " << archive[k].costs[0] << " " << archive[k].costs[1] << " " << archive[k].explored << "<-\n";
								archive.erase(archive.begin() + k);
								k--;
								arch_size--;
							}
						}


						Solution inserted;
						inserted = neighbor;
						if(arch_size > 0)
							inserted.index = archive[arch_size - 1].index + 1;
						else
							inserted.index = 0;
						archive.push_back(inserted);
						arch_size++;
					}

					neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
				}
			}
		}

		//Procurando indice para marcar como explorado
		for(unsigned i(0); i < arch_size; i++){
			if(archive[i].index == current.index){
				archive[i].explored = true;
			}
		}

		//Filtrando soluções não exploradas
		std::vector<Solution> temp;
		for(unsigned i(0); i < arch_size; i++){
			if(archive[i].explored == false){
				temp.push_back(archive[i]);
			}
		}

		archive0 = temp;
		
		//std::cout << "\n" << temp.size() <<  "/" << archive.size() << "\n";


		//std::cout << "tamanho: " << archive0.size() << "\n";

	}while(archive0.size() != 0);

	for(unsigned i(0); i < arch_size; i++){
		archive[i].explored = false;
	}

	archive0 = archive;

	//std::cout << "\nentra1----\n";
}

void MQap::apply_best_exploration(){
	do{
		unsigned arch_size = archive0.size();
		//archive0 = archive;

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
					ohi[i] = 2*ohvc(archive0[sup_index], archive0[i]);
				}else if(sup_index == -1){
					ohi[i] = 2*ohvc(archive0[i], archive0[inf_index]);
				}else{
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
			

		Solution current;
		current = archive0[max_index];
		Solution neighbor;
		neighbor = current;

		
		//Realizando busca na vizinhança por vizinhos que não são diminados pelo arquivo
		//E retirando do arquivos aqueles dominados pelos vizinhos inseridos
		for(int i(0); i < n - 1; i++){
			for(int j(i + 1); j < n; ++j){
				neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
				//Se alguma solução no arquivo domina o vizinho atual, não inseri-lo no arquivo.
				bool non_dominated = true;

				arch_size = archive.size();
				for(unsigned k(0); k < arch_size; k++){
					if(archive[k] < neighbor or archive[k] == neighbor){
						non_dominated = false;
						break;
					}
				}

				//Se a solução é não dominada, retirar os dominados por ela.
				if(non_dominated){
					for(unsigned k(0); k < arch_size; k++){
						if(neighbor < archive[k]){
							//std::cout << archive[k].index << " " << archive[k].costs[0] << " " << archive[k].costs[1] << " " << archive[k].explored << "<-\n";
							archive.erase(archive.begin() + k);
							k--;
							arch_size--;
						}
					}


					Solution inserted;
					inserted = neighbor;
					inserted.index = archive[arch_size - 1].index + 1;
					archive.push_back(inserted);
					arch_size++;
				}

				neighbor.swap_solution(neighbor.solution[i], neighbor.solution[j]);
			}
		}

		//Procurando indice para marcar como explorado
		for(unsigned i(0); i < arch_size; i++){
			if(archive[i].index == current.index){
				archive[i].explored = true;
			}
		}

		//Filtrando soluções não exploradas
		std::vector<Solution> temp;
		for(unsigned i(0); i < arch_size; i++){
			if(archive[i].explored == false){
				temp.push_back(archive[i]);
			}
		}

		archive0 = temp;


	}while(archive0.size() != 0);
	
	//std::cout << "\nentra2----\n";
}

void MQap::anytime_pareto_local_search(){
	//Gerando soluções não dominadas mutuamente.
	archive0.reserve(10000);
	archive.reserve(10000);

	archive0 = generate_non_dominated_solutions();

	int cont = 0;
	for(unsigned i(0); i < archive0.size(); i++){
		archive0[i].index = cont++;
	}

	archive = archive0;

	for(unsigned i(0); i < archive0.size(); i++){
		/*for(unsigned j(0); j < archive0[0].solution.size(); j++){
			std::cout << archive0[i].solution[j] << " ";
		}*/
		std::cout << "\n" << archive0[i].index << " " <<  archive0[i].costs[0] << " " << archive0[i].costs[1] << "\n";
	}


	std::cout << "\n\n-------------------------------------\n\n";

	
	apply_first_exploration();
	apply_best_exploration();

	
	std::cout << "--------------Começo---------------\n";
	for(unsigned i(0); i < archive.size(); i++){
		std::cout << archive[i].index << " " << archive[i].costs[0] << " " << archive[i].costs[1] << "\n";
	}
	std::cout << "--------------Fim---------------\n";
	std::cout << archive.size() << "\n";

}

//Testa se a solução atual do arquivo se tornou dominada por alguma outra
bool MQap::is_dominated_by_archive_post_processing(Solution& solution, int arch_size){
	for(int i(0); i < arch_size; i++){
		if((archive[i].costs[0] < solution.costs[0] and archive[i].costs[1] < solution.costs[1]) or
		(archive[i].costs[0] == solution.costs[0] and archive[i].costs[1] < solution.costs[1]) or
		(archive[i].costs[0] < solution.costs[0] and archive[i].costs[1] == solution.costs[1])){
			return true;
		}
	}

	return false;
}

bool MQap::has_equal_on_archive(Solution& solution, int arch_size){
	for(int i(0); i < arch_size; i++){
		int count = 0;
		for(int j(0); j < n; j++){
			if(archive[i].solution[j] == solution.solution[j]){
				count++;
			}
		}
		if(count == n){
			return true;
		}
	}

	//std::cout << "<----- " << solution.costs[0] << " " << solution.costs[1] << "\n";

	return false;
}

void MQap::path_relinking(){
	anytime_pareto_local_search();
	int arch_size = archive.size();

	for(int i(0); i < arch_size; i++){
		for(int j(0); j < arch_size; j++){
			
			if(i != j){	
				std::vector<Solution> pool;
				Solution candidate;
				Solution last_solution;
				candidate = archive[i];
				last_solution = archive[j];
				
				for (int k = 0; k < n; k++){
					if(candidate.solution[k] != last_solution.solution[k]){
						//std::cout << "*************************************\n";
						//Varrendo o vetor a partir do valor diferente entre a start e last, caso ache, ocorre a troca
						for (int l = k; l < n; l++){
							if(candidate.solution[l] == last_solution.solution[k]){
								candidate.swap_solution(candidate.solution[k], candidate.solution[l]);
								//std::cout << "---" << candidate.index << " " << candidate.costs[0] << " " << candidate.costs[1] << "\n";
								//Armazenando todos as soluções intermediárias.
								pool.push_back(candidate);
							}
						}

						/*if(not is_dominated_by_archive(candidate, arch_size)){
							
							pool.push_back(candidate);
						}*/
						//std::cout << "*************************************\n";
					}
				}
			
			
				for(unsigned k(0); k < pool.size(); k++){
					if(not has_equal_on_archive(pool[k], arch_size)){
						archive.push_back(pool[k]);
						arch_size++;
					}
				}

				/*std::cout << "\ntamanho: " << archive.size() << "\n";
				for(unsigned k(0); k < pool.size(); k++){
					std::cout << "---" << pool[i].index << " " << pool[i].costs[0] << " " << pool[i].costs[1] << "\n";
				}*/

				for(int k(0); k < arch_size; k++){
					if(is_dominated_by_archive_post_processing(archive[k], arch_size)){
						//std::cout << "\napaga" << "\n";
						archive.erase(archive.begin() + k);
						arch_size--;
						k--;
					}
				}

				//std::cout << archive.size() <<"<<<<<<<<<<<<<<<<<<<<<<<<<\n";
			}
		}
		
	}

	std::cout << "\n";
	for(unsigned i(0); i < archive.size(); i++){
		std::cout << archive[i].index << " " << archive[i].costs[0] << " " << archive[i].costs[1] << "\n";
	}

	std::cout << archive.size() << "\n";
	
}