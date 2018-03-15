#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>
#include <math.h>

#define D2R 3.14159265358979323846/180
#define R_MAX = 300

/*Estrutura de dados que guarda as informacoes do .dat*/
typedef struct{
  int n;    // Número de localidades
  int *c;   // Vetor com custos de abrir cada facilidade
  int **a;  // Matrix binaria onde 1-> se a localidade j está dentro do raio de cobertura da localidade j
}Data;

/*Função que aplica que calcula a distancia esferica entre duas localidades
n -> Número de localidades
lat1, lng1 -> Latitude e longitude da primeira facilidade
lat2, lng2 -> Latitude e longitude da segunda facilidade */
float calc_dist(int n, float lat1, float lng1, float lat2, float lng2){
  float alpha;
  float dist[n][n];

  alpha = sin(D2R*(lat1-lat2)/2)**2 + cos(D2R*lat1)*cos(D2R*lat2)*sin(D2r*(lng1-lng2)/2)**2;
  dist = 2*6371*atan2(sqrt(alpha), sqrt(1-alpha));
  return dist;

}

/*Função que lê o arquivo de dados e retorna uma struct com os parametros necessários para resolução do
problema MIP
f_nome -> Nome do arquivo .dat*/
Data le_arquivo(char f_name){
  Data data;
  FILE *f = fopen(f_name,"r");

  /*Alocacao dinamica de memoria*/
  fscanf(f,"%d",&data.n);
  data.c = (int*) malloc (data.n * sizeof(int));
  data.a = (int**) malloc (data.n*sizeof(int*));
  for(int i=0; i < data.n; i++){
    data.a[i] = (int*) malloc (data.n*sizeof(int));
  }
  float *lat = (float*) malloc (data.n * sizeof(float));
  float *lng = (float*) malloc (data.n * sizeof(float));

  /*Leitura do arquivo*/
  for(int i=0; i < data.n;i++){
    fscanf(f,"%d ",&data.c[i]);
  }
  for(int i=0; i <data.n; i++){
    fscanf(f,"%f,%f ",&lat[i],&lng[i]);
  }
  /*Geração da matrix de cobertura a com base na distancia esferica*/
  for(int i=0; i < data.n; i++){
    for(int j = 0; j < data.n; j++){
      if(calc_dist(data.n,lat[i],lng[i], lat[j],lng[j]) <= R_MAX){
        data.a[i][j] == 1;
      }
      else{
        data.a[i][j] == 0;
      }
    }
  }
  free(lat);
  free(lng);
  return data;
}

int main(void){
  /*Declaracao de variaveis*/
  glp_prob *lp;
  Data data = le_arquivo(f);
  int ia[1+1000], ja[1+1000];
  int *x = (int*) malloc ((data.n+1) * sizeof(int));
  int aux;
  double ar[1+1000], z;
  char f_nome[]= "sc.dat";

  /* Criacao do priblema */
  lp = glp_create_prob();
  glp_set_prob_name(lp, "set_covering");
  glp_set_obj_dir(lp, GLP_MIN);

  /* Adicionando linhas (restricoes) */
  glp_add_rows(lp, data.n);
  for (int aux=1; aux<=data.n; aux++){
    //glp_set_row_name(lp, aux, "r%d");
    glp_set_row_bnds(lp, aux, GLP_LO,1.0,0.0);
  }
  /* Adicionando columnas (variaveis) */
  glp_add_cols(lp,data.n);
  for (int aux=1; aux<=data.n; aux++){
    //glp_set_col_name(lp, aux, "x%d");
    glp_set_col_kind(lp, aux, GLP_BV);
  }

  /* Adicionando funcao objetivo */
  for(int aux=1; aux<=data.n; aux++){
    glp_set_obj_coef(lp,aux,data.c[aux]);
  }

  /* Adicionando coeficientes das restricoes */
  for(int i=1; i<=data.n;i++){
    for(int j=1; j<=data.n;j++){
      ia[aux] = i, ja[aux] = j, ar[aux] = data.a[i-1][j-1];
      aux++;
    }
  }
  glp_load_matrix(lp, aux, ia, ja, ar);

  /* Obtencao da solucao apartir do metodo de branch and cut */
  glp_intopt(lp, NULL);

  /* Display dos resultados */
  z = glp_get_obj_val(lp);
  printf("função objetivo: %g\n",z);
  for(int i=1; i<=data.n;i++){
    x[i] = glp_get_col_prim(lp,i);
    printf("x%d = %d,  ",i,x[i]);
  }

  /* Desalocacao de memoria */
  glp_delete_prob(lp);
  glp_free_env();
  free(x);

  return 0;
}
