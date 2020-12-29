# Anytime pareto local search aplicado ao problema quadrático de alocação multiobjetivo (PQAM).

## Input

Como entrada para o algoritmo foram usadas intâncias geradas pelo [algoritmo](https://www.cs.bham.ac.uk/~jdk/mQAP/) desenvolvido por Joshua D. Knowles.  O algoritmo gerou instâncias de tamanho 30 e de correlação entre a matriz de fluxo 1 e 2 de valor -0.75.

Os algoritmo são os generator.cc e real_like.cc do repositório.

## Detalhes do algoritmo
- Foi determinado que o problema teria dois objetivos calculados com a função objetivo convencional do PQA utilizando as duas matrizes (30 x 30) de fluxo para calcular os dois valores, além da matriz de distância.

- Sendo assim uma solução para o problema terá, além do vetor solução (de tamanho 30) do problema, os dois valores relativos aos custos calculados pelas funções de cada objetivo. Também tem-se uma flag que marca se a solução é explorada ou não.

- Foi criado um operador '>' para verificar se é o caso em, por exemplo, A > B, que a solução A domina a solução B.

## Descrição do loop principal do algoritmo
Temporariamente foi decidido implementar a versão convencional da pareto local search já que partes dela serão usadas na versão anytime.
Os seguintes passos foram realizados usando o [pseudocódigo](https://www.researchgate.net/publication/273400799_Anytime_Pareto_local_search) convencional da pareto local search: 

- Primeiro criou-se um conjunto de soluções não dominadas mutuamente utilizando como base um [artigo](https://www.researchgate.net/publication/43789438_A_Fast_Algorithm_for_Finding_the_non_Dominated_Set_in_Multiobjective_Optimization) que descreve uma maneira eficiente de gerar esse conjunto.
- A parte de seleção da solução a ser explorada em cada iteração é realizada aleatoriamente a partir do 'arquivo', que inicialmente é o próprio conjunto não dominado.

- O critério de aceitação da solução para o arquivo é de que a solução considerada não seja dominada por nenhuma solução do 'arquivo'.
 
 - Após uma solução ser aceita no arquivo deve-se excluir as que se tornam dominadas por ela.
 
- Para gerar a vizinhança de uma solução foi utilizado o algoritmo 2-opt.

- A condição de parada é a de que todas as soluções do 'arquivo' sejam exploradas.

Todos esses passos se encontram no método anytime_pareto_local_search do arquivo mQap.cpp.

## Problema encontrados atualmente
- O algoritmo aparentemente aceita mais soluções do que apaga do arquivo de forma que o algoritmo atualmente não para.
