# Implementação de Modelo para Set Cover em C (glpk)

Projeto desenvolvido por grupo de alunos da Engenharia de Produção da Universidade Federal de Minas Gerais (UFMG) para primeira parte de um trabalho de disciplina de Logistica. O trabalho consiste na implementação de um modelo de programação linear para a versão de otimização do problema Set Cover. O grupo optou por implementar em C utilizando o solver `glpk`.

## Pré-requisitos

Para compilar o códido em C é necessário a instalação da biblioteca do glpk que pode ser obtida em: http://ftp.gnu.org/gnu/glpk/. Um .mod com uma versão em gmath do modelo se encontra em `set_cover.mod`.

## Set Cover
O Set Cover é um problema de decisão classificado como NP-Completo que pode ser definido da seguinte forma:
Dado um conjunto **S** de pontos, um raio **R** de cobertura e um número inteiro *k* determinar se  possível escolher um sub-conjunto de **S** com *k* elementos de forma que todos os demais pontos estajam a uma distância de no máximo **R** de pelo menos um dos pontos selecionados.

Nesse trabalho uma versão de otimização desse problema em que cada ponto está associado com um "custo de abertura", e o objetivo e escolher o conjunto que atende a restrição do set cover e tem menor custo total.
