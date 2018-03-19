#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>
#include <math.h>

#define D2R 3.14159265358979323846/180
#define R_MAX 100

/*Estrutura de dados que guarda as informacoes do .dat*/
typedef struct{
  int n;         // Número de localidades
  char **name; //Nome da cidade
  int *c;        // Vetor com custos de abrir cada facilidade
  int **a;       // Matrix binaria onde 1-> se a localidade j está dentro do raio de cobertura da localidade j
}Data;

/*Função que aplica que calcula a distancia esferica entre duas localidades
n -> Número de localidades
lat1, lng1 -> Latitude e longitude da primeira facilidade
lat2, lng2 -> Latitude e longitude da segunda facilidade */
float calc_dist(int n, float lat1, float lng1, float lat2, float lng2){
  float alpha;
  float dist;

  alpha = pow(sin(D2R*(lat1-lat2)/2),2) + cos(D2R*lat1)*cos(D2R*lat2)*pow(sin(D2R*(lng1-lng2)/2),2);
  dist = 2*6371*atan2(sqrt(alpha), sqrt(1-alpha));
  return dist;

}

/*Função que lê o arquivo de dados e retorna uma struct com os parametros necessários para resolução do
problema MIP
f_nome -> Nome do arquivo .dat*/
Data le_arquivo(char *f_name){
  Data data;
  FILE *f = fopen(f_name,"r");

  /*Alocacao dinamica de memoria*/
  fscanf(f,"%d",&data.n);
  data.name = (char**) malloc (data.n*sizeof(char*));
  for(int i=0; i < data.n; i++){
    data.name[i] = (char*) malloc (data.n*30*sizeof(char));
  }
  data.c = (int*) malloc (data.n * sizeof(int));
  data.a = (int**) malloc (data.n*sizeof(int*));
  for(int i=0; i < data.n; i++){
    data.a[i] = (int*) malloc (data.n*sizeof(int));
  }
  float *lat = (float*) malloc (data.n * sizeof(float));
  float *lng = (float*) malloc (data.n * sizeof(float));

  /*Leitura do arquivo*/
  for(int i=0; i < data.n;i++){
    fscanf(f,"%s, ", data.name[i]);
    //printf("%s-%d\n",data.name[i],i);
  }
  for(int i=0; i < data.n;i++){
    fscanf(f,"%d,", &data.c[i]);
    //printf("%d ", data.c[i]);
  }
  printf("\n");
  for(int i=0; i <data.n; i++){
    fscanf(f,"%f,%f ",&lat[i],&lng[i]);
    //printf("%f ", lat[i]);
  }
  /*Geração da matrix de cobertura a com base na distancia esferica*/
  for(int i=0; i < data.n; i++){
    for(int j = 0; j < data.n; j++){
      if(calc_dist(data.n,lat[i],lng[i], lat[j],lng[j]) <= R_MAX){
        data.a[i][j] = 1;
      }
      else{
        //printf("\n\n\n\n ENTROOOOOOUUUUUU \n\n\n\n\n");
        data.a[i][j] = 0;
      }
    }
  }
  free(lat);
  free(lng);
  fclose(f);
  return data;
}

int main(void){
  /*Declaracao de variaveis*/
  glp_prob *lp;
  char f_name[]= "sc.dat";
  Data data = le_arquivo(f_name);
  int *x = (int*) malloc ((data.n+1) * sizeof(int));

  int ia[1000+1], ja[1000+1];
  double ar[1000+1], z;

  /* Criacao do priblema */
  lp = glp_create_prob();
  glp_set_prob_name(lp, "set_covering");
  glp_set_obj_dir(lp, GLP_MIN);

  /* Adicionando linhas (restricoes) */
  glp_add_rows(lp, data.n);
  for (int i=1; i<=data.n; i++){
    //glp_set_row_name(lp, i, "r%d");
    glp_set_row_bnds(lp, i, GLP_LO, 1.0, 0.0);
  }

  /* Adicionando colunas (variaveis) */
  glp_add_cols(lp,data.n);
  for (int j=1; j<=data.n; j++){
    //glp_set_col_name(lp, aux, "x%d");
    glp_set_col_kind(lp, j, GLP_BV);
  }

  /* Adicionando funcao objetivo */
  for(int i=1; i<=data.n; i++){
    //printf("%d:%d ",i, data.c[i-1]);
    glp_set_obj_coef(lp,i,data.c[i-1]);
  }

  /* Adicionando coeficientes das restricoes */
  int aux = 0;
  for(int i=1; i<=data.n;i++){
    for(int j=1; j<=data.n;j++){
      aux++;
      ia[aux] = i, ja[aux] = j, ar[aux] = data.a[i-1][j-1];
    }
  }
  glp_load_matrix(lp, aux, ia, ja, ar);

  /* Obtencao da solucao apartir do metodo de branch and cut */
  glp_iocp param;
  glp_init_iocp(&param);
  param.presolve = GLP_ON;
  glp_intopt(lp, &param);

  /* Display dos resultados */
  int sum = 0;
  z = glp_mip_obj_val(lp);
  for(int i=1; i<=data.n;i++){
    x[i] = glp_mip_col_val(lp,i);
    if(x[i] == 1){
      sum ++;
      printf("\n%s -> %d",data.name[i-1],data.c[i-1]);
    }
    //printf("x%d = %d,  ",i,x[i]);
  }
  printf("\n\nFunção Objetivo: %g",z);
  printf("\nNúmero de facilidades abertas: %d\n",sum);

  /* Desalocacao de memoria */
  glp_delete_prob(lp);
  glp_free_env();
  free(x);
  for(int i = 0; i<data.n; i++) free(data.name[i]);
  free(data.name);
  free(data.c);
  for(int i = 0; i<data.n; i++) free(data.a[i]);
  free(data.a);

  return 0;
}
