Programas necess�rios para rodar o c�digo em C:

- Compilador: (Usamos o gcc durante os testes)
- Biblioteca do glpk para C (glpk.h). Pode-se obt�-la atrav�s do seguinte seguite: http://ftp.gnu.org/gnu/glpk/

Por fim para compilar e executar o programa no Ubuntu foi utilizado o seguinte comando:

gcc set_cover.c -lglpk -lm -o out
./out 
