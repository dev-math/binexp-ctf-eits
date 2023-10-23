#include <stdio.h>

struct {
	char nome[32];
	int id;
} usuario = {.nome = {0}, .id = 2};

char *mensagens[] = {
	"eits{}",
	"voce nao tem permissao de leitura"
};

int main(void)
{
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
  printf("%lu\n", sizeof(char));
  printf("%lu\n", sizeof(usuario));
	printf("Qual o seu nome?\n> ");
	scanf("%s", usuario.nome);
	printf("id=%d\n", usuario.id);
	printf("Ola, %s\n", usuario.nome);

	if (usuario.id <= 0 || usuario.id > (int) sizeof(mensagens)) {
		printf("Usuario inexistente\n");
	}
	else {
    // usuario.id == 1 ou 2
    // queremos o id == 1
		printf("Mensagem: %s\n", mensagens[usuario.id - 1]);
	}
}
