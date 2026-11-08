
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_ITEMS 1000
#define MAX_NAME 100
#define FILE_NAME "inventory.txt"

typedef struct {
    char nome[MAX_NAME];
    int quantidade;
} Item;

// Remove a quebra de linha do final de uma string
void remover_quebra_linha(char texto[]) {
    texto[strcspn(texto, "\r\n")] = '\0';
}

// Le entrada de texto
int ler_texto(const char *mensagem, char texto[], int tamanho) {
    printf("%s", mensagem);

    if (fgets(texto, tamanho, stdin) == NULL) {
        return 0;
    }

    if (strchr(texto, '\n') == NULL) {
        int caractere;

        while ((caractere = getchar()) != '\n' && caractere != EOF);
    }

    remover_quebra_linha(texto);

    return 1;
}

// Converte uma string em inteiro, rejeitando caracteres extras
int converter_inteiro(const char texto[], int *valor) {
    char *fim;
    long numero = strtol(texto, &fim, 10);

    if (texto == fim) {
        return 0;
    }

    while (isspace((unsigned char)*fim)) {
        fim++;
    }

    if (*fim != '\0' || numero < INT_MIN || numero > INT_MAX) {
        return 0;
    }

    *valor = (int)numero;

    return 1;
}

// Le uma quantidade inteira positiva
int ler_quantidade(const char *mensagem) {
    char entrada[100];
    int quantidade;

    while (1) {
        if (!ler_texto(mensagem, entrada, sizeof(entrada))) {
            return -1;
        }

        if (converter_inteiro(entrada, &quantidade) && quantidade > 0) {
            return quantidade;
        }

        printf("Quantidade invalida! Digite um numero inteiro positivo.\n");
    }
}

// Carrega os produtos do arquivo
int carregar_estoque(Item estoque[]) {
    FILE *arquivo = fopen(FILE_NAME, "r");
    char linha[MAX_NAME];
    char quantidade_texto[100];
    int quantidade;
    int total = 0;

    if (arquivo == NULL) {
        return 0;
    }

    while (total < MAX_ITEMS &&
           fgets(linha, sizeof(linha), arquivo) != NULL) {

        remover_quebra_linha(linha);

        if (fgets(quantidade_texto, sizeof(quantidade_texto), arquivo) == NULL) {
            break;
        }

        remover_quebra_linha(quantidade_texto);

        if (converter_inteiro(quantidade_texto, &quantidade) &&
            quantidade >= 0 && linha[0] != '\0') {

            strcpy(estoque[total].nome, linha);
            estoque[total].quantidade = quantidade;

            total++;
        }
    }

    fclose(arquivo);

    return total;
}

// Salva todos os produtos no arquivo
int salvar_estoque(Item estoque[], int total) {
    FILE *arquivo = fopen(FILE_NAME, "w");

    if (arquivo == NULL) {
        return 0;
    }

    for (int i = 0; i < total; i++) {
        fprintf(arquivo, "%s\n", estoque[i].nome);
        fprintf(arquivo, "%d\n", estoque[i].quantidade);
    }

    fclose(arquivo);

    return 1;
}

// Adiciona um produto ao estoque
void adicionar_item(Item estoque[], int *total) {
    char nome[MAX_NAME];
    int quantidade;

    if (*total >= MAX_ITEMS) {
        printf("Limite de produtos atingido.\n");
        return;
    }

    if (!ler_texto("Digite o nome do item: ", nome, sizeof(nome))) {
        return;
    }

    if (nome[0] == '\0') {
        printf("O nome do item nao pode ficar vazio.\n");
        return;
    }

    quantidade = ler_quantidade("Digite a quantidade: ");

    if (quantidade <= 0) {
        return;
    }

    strcpy(estoque[*total].nome, nome);
    estoque[*total].quantidade = quantidade;

    (*total)++;

    if (salvar_estoque(estoque, *total)) {
        printf("Item adicionado com sucesso!\n");
    } else {
        printf("Erro ao salvar o arquivo inventory.txt.\n");
    }
}

// Remove uma quantidade de um produto
void remover_item(Item estoque[], int *total) {
    char nome[MAX_NAME];
    int quantidade;
    int encontrado = -1;

    if (!ler_texto("Digite o nome do item: ", nome, sizeof(nome))) {
        return;
    }

    quantidade = ler_quantidade("Digite a quantidade a ser removida: ");

    if (quantidade <= 0) {
        return;
    }

    // Procura o produto no estoque
    for (int i = 0; i < *total; i++) {
        if (strcmp(estoque[i].nome, nome) == 0) {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1) {
        printf("Item nao encontrado.\n");
        return;
    }

    // Verifica se existe quantidade suficiente
    if (quantidade > estoque[encontrado].quantidade) {
        printf("Estoque insuficiente. Quantidade disponivel: %d\n",
               estoque[encontrado].quantidade);
        return;
    }

    // Atualiza a quantidade
    estoque[encontrado].quantidade -= quantidade;

    // Remove o produto se a quantidade chegar a zero
    if (estoque[encontrado].quantidade == 0) {

        for (int i = encontrado; i < *total - 1; i++) {
            estoque[i] = estoque[i + 1];
        }

        (*total)--;

        if (salvar_estoque(estoque, *total)) {
            printf("Item removido do estoque!\n");
        } else {
            printf("Erro ao salvar o arquivo inventory.txt.\n");
        }

    } else {

        if (salvar_estoque(estoque, *total)) {
            printf("Quantidade atualizada com sucesso!\n");
        } else {
            printf("Erro ao salvar o arquivo inventory.txt.\n");
        }
    }
}

// Exibe todos os produtos cadastrados
void listar_estoque(Item estoque[], int total) {

    printf("=============================\n");
    printf("       Estoque Atual\n");
    printf("=============================\n");

    if (total == 0) {
        printf("O estoque esta vazio.\n");
        return;
    }

    for (int i = 0; i < total; i++) {
        printf("Nome: %s\n", estoque[i].nome);
        printf("Quantidade: %d\n\n", estoque[i].quantidade);
    }
}

int main(void) {
    Item estoque[MAX_ITEMS];
    int total;
    int opcao;
    char entrada[100];
    char resposta[100];

    while (1) {

        // Carrega os dados salvos
        total = carregar_estoque(estoque);

        // Menu principal
        printf("================================\n");
        printf("     Controle de Estoque\n");
        printf("================================\n");
        printf("Selecione uma opcao:\n");
        printf("1. Adicionar Item\n");
        printf("2. Remover Item\n");
        printf("3. Listar Estoque\n");
        printf("4. Sair\n");
        
        // Validacao da opcao
        while (1) {
            if (!ler_texto("Opcao: ", entrada, sizeof(entrada))) {
                printf("Obrigado por usar o Controle de Estoques! Ate a proxima.\n");
                return 0;
            }

            if (converter_inteiro(entrada, &opcao) &&
                opcao >= 1 && opcao <= 4) {
                break;
            }

            printf("Opcao invalida! Tente novamente.\n");
        }

        // Executa a opcao escolhida
        switch (opcao) {

            case 1:
                adicionar_item(estoque, &total);
                break;

            case 2:
                remover_item(estoque, &total);
                break;

            case 3:
                listar_estoque(estoque, total);
                break;

            case 4:
                printf("Obrigado por usar o Controle de Estoques! Ate a proxima.\n");
                return 0;
        }

        // Pergunta se deseja continuar
        if (!ler_texto("Deseja realizar outra operacao? (s/n): ",
                       resposta, sizeof(resposta))) {
            printf("Obrigado por usar o Controle de Estoques! Ate a proxima.\n");
            return 0;
        }

        if (tolower((unsigned char)resposta[0]) != 's' ||
            resposta[1] != '\0') {

            printf("Obrigado por usar o Controle de Estoques! Ate a proxima.\n");
            return 0;
        }

        printf("\n");
    }

    return 0;
}
