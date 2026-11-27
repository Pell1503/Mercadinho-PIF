#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUTOS   100
#define MAX_ITENS       50
#define TAM_NOME        40
#define ESTOQUE_BAIXO    5
#define COD_INICIAL   1001
#define LIM_QTD      10000
#define LIM_PRECO  1000000

typedef long long Centavos;

typedef struct {
    int      codigo;
    char     nome[TAM_NOME];
    Centavos preco;
    int      estoque;
    int      vendidos;
    Centavos faturado;
} Produto;

typedef struct {
    Produto *produto;
    int      quantidade;
    Centavos subtotal;
} ItemVenda;

typedef struct {
    int      numVendas;
    int      itensVendidos;
    Centavos faturamento;
    Centavos maiorVenda;
    Centavos totalTroco;
} ResumoDia;

static void     lerLinha(const char *msg, char *buf, int n);
static int      lerInt(const char *msg, int min, int max);
static Centavos lerCentavos(const char *msg, Centavos min);

static void     formatarReais(Centavos c, char *saida, size_t n);
static int      igualIgnoraCaso(const char *a, const char *b);
static int      comparaIgnoraCaso(const char *a, const char *b);
static int      contemIgnoraCaso(const char *texto, const char *busca);
static int      soDigitos(const char *s);
static void     pausaTitulo(const char *titulo);

static Produto *buscarPorCodigo(Produto *lista, int total, int codigo);
static int      adicionarProduto(Produto *lista, int *total, const char *nome,
                                 Centavos preco, int estoque);
static void     cadastrarProduto(Produto *lista, int *total);
static void     entradaEstoque(Produto *lista, int total);
static void     listarProdutos(Produto *lista, int total);
static void     consultarProduto(Produto *lista, int total);
static void     detalharProduto(const Produto *p);
static void     carregarExemplos(Produto *lista, int *total);

static ItemVenda *encontrarItem(ItemVenda *itens, int n, const Produto *p);
static void     mostrarCarrinho(const ItemVenda *itens, int n, Centavos total);
static void     realizarVenda(Produto *lista, int total, ResumoDia *dia);
static void     relatorioDia(const Produto *lista, int total, const ResumoDia *dia);

static void     mostrarMenu(void);

static void lerLinha(const char *msg, char *buf, int n)
{
    size_t len;

    printf("%s", msg);
    fflush(stdout);

    if (fgets(buf, n, stdin) == NULL) {
        printf("\n[Entrada encerrada. Saindo do sistema.]\n");
        exit(0);
    }

    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {

        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }

    len = strlen(buf);
    while (len > 0 && isspace((unsigned char)buf[len - 1])) {
        buf[--len] = '\0';
    }
    {
        char *ini = buf;
        while (*ini != '\0' && isspace((unsigned char)*ini)) ini++;
        if (ini != buf) memmove(buf, ini, strlen(ini) + 1);
    }
}

static int lerInt(const char *msg, int min, int max)
{
    char  buf[64];
    char *fim;
    long  v;

    for (;;) {
        lerLinha(msg, buf, sizeof buf);
        v = strtol(buf, &fim, 10);

        if (buf[0] == '\0' || fim == buf || *fim != '\0') {
            printf("  Entrada invalida. Digite um numero inteiro.\n");
        } else if (v < min || v > max) {
            printf("  Valor fora do intervalo (%d a %d).\n", min, max);
        } else {
            return (int)v;
        }
    }
}

static Centavos lerCentavos(const char *msg, Centavos min)
{
    char     buf[64];
    char    *fim, *p;
    double   v;
    Centavos c;

    for (;;) {
        lerLinha(msg, buf, sizeof buf);
        for (p = buf; *p != '\0'; p++) {
            if (*p == ',') *p = '.';
        }
        v = strtod(buf, &fim);

        if (buf[0] == '\0' || fim == buf || *fim != '\0') {
            printf("  Entrada invalida. Exemplo: 12,50\n");
            continue;
        }
        if (!(v >= 0.0 && v <= 1000000.0)) {
            printf("  Valor fora do limite permitido.\n");
            continue;
        }
        c = (Centavos)(v * 100.0 + 0.5);
        if (c < min) {
            char tmp[32];
            formatarReais(min, tmp, sizeof tmp);
            printf("  O valor deve ser de pelo menos %s.\n", tmp);
            continue;
        }
        return c;
    }
}

static void formatarReais(Centavos c, char *saida, size_t n)
{

    snprintf(saida, n, "R$ %ld,%02d", (long)(c / 100), (int)(c % 100));
}

static int igualIgnoraCaso(const char *a, const char *b)
{
    return comparaIgnoraCaso(a, b) == 0;
}

static int comparaIgnoraCaso(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

static int contemIgnoraCaso(const char *texto, const char *busca)
{
    size_t n = strlen(busca);
    size_t i;

    if (n == 0) return 1;
    for (; *texto != '\0'; texto++) {
        i = 0;
        while (i < n && texto[i] != '\0' &&
               tolower((unsigned char)texto[i]) == tolower((unsigned char)busca[i])) {
            i++;
        }
        if (i == n) return 1;
    }
    return 0;
}

static int soDigitos(const char *s)
{
    if (*s == '\0') return 0;
    for (; *s != '\0'; s++) {
        if (!isdigit((unsigned char)*s)) return 0;
    }
    return 1;
}

static void pausaTitulo(const char *titulo)
{
    printf("\n=== %s ===\n", titulo);
}

static Produto *buscarPorCodigo(Produto *lista, int total, int codigo)
{
    int i;
    for (i = 0; i < total; i++) {
        if (lista[i].codigo == codigo) {
            return &lista[i];
        }
    }
    return NULL;
}

static int adicionarProduto(Produto *lista, int *total, const char *nome,
                            Centavos preco, int estoque)
{
    int i;
    Produto *novo;

    if (nome[0] == '\0' || preco <= 0 || estoque < 0) return 3;
    if (*total >= MAX_PRODUTOS) return 1;

    for (i = 0; i < *total; i++) {
        if (igualIgnoraCaso(lista[i].nome, nome)) return 2;
    }

    novo = &lista[*total];
    novo->codigo   = COD_INICIAL + *total;
    strncpy(novo->nome, nome, TAM_NOME - 1);
    novo->nome[TAM_NOME - 1] = '\0';
    novo->preco    = preco;
    novo->estoque  = estoque;
    novo->vendidos = 0;
    novo->faturado = 0;
    (*total)++;
    return 0;
}

static void cadastrarProduto(Produto *lista, int *total)
{
    char     nome[TAM_NOME];
    Centavos preco;
    int      estoque, r;

    pausaTitulo("CADASTRO DE PRODUTO");

    if (*total >= MAX_PRODUTOS) {
        printf("Limite de %d produtos atingido.\n", MAX_PRODUTOS);
        return;
    }

    do {
        lerLinha("Nome do produto: ", nome, sizeof nome);
        if (nome[0] == '\0') {
            printf("  O nome nao pode ser vazio.\n");
        }
    } while (nome[0] == '\0');

    do {
        preco = lerCentavos("Preco unitario (R$): ", 1);
        if (preco > LIM_PRECO) {
            printf("  Preco maximo permitido: R$ %d,00.\n", LIM_PRECO / 100);
        }
    } while (preco > LIM_PRECO);
    estoque = lerInt("Estoque inicial: ", 0, LIM_QTD);

    r = adicionarProduto(lista, total, nome, preco, estoque);
    if (r == 0) {
        printf("Produto cadastrado com sucesso! Codigo: %d\n",
               lista[*total - 1].codigo);
    } else if (r == 2) {
        printf("Erro: ja existe um produto com esse nome.\n");
    } else {
        printf("Erro: nao foi possivel cadastrar o produto.\n");
    }
}

static void entradaEstoque(Produto *lista, int total)
{
    int      codigo, qtd;
    Produto *p;

    pausaTitulo("ENTRADA EM ESTOQUE");

    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    codigo = lerInt("Codigo do produto (0 = voltar): ", 0, 999999);
    if (codigo == 0) return;

    p = buscarPorCodigo(lista, total, codigo);
    if (p == NULL) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Produto: %s | estoque atual: %d\n", p->nome, p->estoque);
    qtd = lerInt("Quantidade a adicionar: ", 1, LIM_QTD);

    if (p->estoque + qtd > 1000000) {
        printf("Erro: estoque total excederia o limite.\n");
        return;
    }
    p->estoque += qtd;
    printf("Entrada registrada. Novo estoque de %s: %d\n", p->nome, p->estoque);
}

static void listarProdutos(Produto *lista, int total)
{
    Produto *ord[MAX_PRODUTOS];
    Produto *tmp;
    int      i, j, modo;
    char     preco[32];

    pausaTitulo("LISTA DE PRODUTOS");

    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    modo = lerInt("Ordenar por: 1) Codigo  2) Nome : ", 1, 2);

    for (i = 0; i < total; i++) ord[i] = &lista[i];

    if (modo == 2) {
        for (i = 0; i < total - 1; i++) {
            for (j = 0; j < total - 1 - i; j++) {
                if (comparaIgnoraCaso(ord[j]->nome, ord[j + 1]->nome) > 0) {
                    tmp = ord[j];
                    ord[j] = ord[j + 1];
                    ord[j + 1] = tmp;
                }
            }
        }
    }

    printf("\n%-6s %-30s %12s %8s\n", "COD", "NOME", "PRECO", "ESTOQUE");
    printf("------------------------------------------------------------------\n");
    for (i = 0; i < total; i++) {
        formatarReais(ord[i]->preco, preco, sizeof preco);
        printf("%-6d %-30.30s %12s %8d", ord[i]->codigo, ord[i]->nome,
               preco, ord[i]->estoque);
        if (ord[i]->estoque == 0) {
            printf("  [ZERADO]");
        } else if (ord[i]->estoque <= ESTOQUE_BAIXO) {
            printf("  [BAIXO]");
        }
        printf("\n");
    }
    printf("\nTotal de produtos cadastrados: %d\n", total);
}

static void detalharProduto(const Produto *p)
{
    char preco[32], fat[32];
    formatarReais(p->preco, preco, sizeof preco);
    formatarReais(p->faturado, fat, sizeof fat);
    printf("  Codigo ........: %d\n", p->codigo);
    printf("  Nome ..........: %s\n", p->nome);
    printf("  Preco .........: %s\n", preco);
    printf("  Estoque .......: %d\n", p->estoque);
    printf("  Vendidos hoje .: %d (%s)\n", p->vendidos, fat);
}

static void consultarProduto(Produto *lista, int total)
{
    char     termo[TAM_NOME];
    Produto *p;
    int      i, achou = 0;

    pausaTitulo("CONSULTA DE PRODUTO");

    if (total == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    lerLinha("Digite o codigo ou parte do nome: ", termo, sizeof termo);
    if (termo[0] == '\0') {
        printf("Nada digitado.\n");
        return;
    }

    if (soDigitos(termo)) {
        p = buscarPorCodigo(lista, total, atoi(termo));
        if (p != NULL) {
            printf("\nProduto encontrado:\n");
            detalharProduto(p);
            return;
        }
    }

    for (i = 0; i < total; i++) {
        if (contemIgnoraCaso(lista[i].nome, termo)) {
            if (!achou) printf("\nProdutos encontrados:\n");
            achou++;
            printf("\n");
            detalharProduto(&lista[i]);
        }
    }
    if (!achou) {
        printf("Nenhum produto encontrado para \"%s\".\n", termo);
    } else {
        printf("\n%d produto(s) encontrado(s).\n", achou);
    }
}

static void carregarExemplos(Produto *lista, int *total)
{
    static const struct {
        const char *nome;
        Centavos    preco;
        int         estoque;
    } ex[] = {
        {"Arroz 5kg",          2590, 20},
        {"Feijao 1kg",          849, 30},
        {"Acucar 1kg",          459, 25},
        {"Cafe 500g",          1290, 15},
        {"Leite Integral 1L",   549, 40},
        {"Biscoito Recheado",   299,  3},
        {"Refrigerante 2L",     899, 12},
        {"Sabao em Po 1kg",    1890,  8}
    };
    int i, n = (int)(sizeof ex / sizeof ex[0]), novos = 0;

    for (i = 0; i < n; i++) {
        if (adicionarProduto(lista, total, ex[i].nome, ex[i].preco,
                             ex[i].estoque) == 0) {
            novos++;
        }
    }
    printf("%d produto(s) de exemplo carregado(s).\n", novos);
}

static ItemVenda *encontrarItem(ItemVenda *itens, int n, const Produto *p)
{
    int i;
    for (i = 0; i < n; i++) {
        if (itens[i].produto == p) return &itens[i];
    }
    return NULL;
}

static void mostrarCarrinho(const ItemVenda *itens, int n, Centavos total)
{
    int  i;
    char unit[32], sub[32];

    printf("\n  --------------- CARRINHO ---------------\n");
    if (n == 0) printf("  (vazio)\n");
    for (i = 0; i < n; i++) {
        formatarReais(itens[i].produto->preco, unit, sizeof unit);
        formatarReais(itens[i].subtotal, sub, sizeof sub);
        printf("  %2d. %-22.22s %4d x %11s = %12s\n", i + 1,
               itens[i].produto->nome, itens[i].quantidade, unit, sub);
    }
    formatarReais(total, unit, sizeof unit);
    printf("  TOTAL PARCIAL: %s\n\n", unit);
}

static void realizarVenda(Produto *lista, int total, ResumoDia *dia)
{
    ItemVenda itens[MAX_ITENS];
    int       nItens = 0, codigo, qtd, i, unidades = 0;
    Centavos  totalVenda = 0, pago, troco;
    Produto  *p;
    ItemVenda *it;
    char      a[32], b[32], c[32];

    pausaTitulo("NOVA VENDA");

    if (total == 0) {
        printf("Nenhum produto cadastrado. Cadastre produtos antes de vender.\n");
        return;
    }

    printf("Codigo 0 = finalizar | -1 = cancelar venda | -2 = remover ultimo item\n");

    for (;;) {
        codigo = lerInt("\nCodigo do produto: ", -2, 999999);

        if (codigo == 0) break;

        if (codigo == -1) {
            printf("Venda cancelada. Nenhum estoque foi alterado.\n");
            return;
        }

        if (codigo == -2) {
            if (nItens == 0) {
                printf("O carrinho esta vazio.\n");
            } else {
                nItens--;
                totalVenda -= itens[nItens].subtotal;
                printf("Item removido: %s\n", itens[nItens].produto->nome);
                mostrarCarrinho(itens, nItens, totalVenda);
            }
            continue;
        }

        p = buscarPorCodigo(lista, total, codigo);
        if (p == NULL) {
            printf("Produto nao encontrado.\n");
            continue;
        }

        it = encontrarItem(itens, nItens, p);
        {
            int reservado  = (it != NULL) ? it->quantidade : 0;
            int disponivel = p->estoque - reservado;

            formatarReais(p->preco, a, sizeof a);
            printf("%s - %s - disponivel: %d\n", p->nome, a, disponivel);

            if (disponivel <= 0) {
                printf("Produto sem estoque disponivel.\n");
                continue;
            }

            qtd = lerInt("Quantidade: ", 1, LIM_QTD);
            if (qtd > disponivel) {
                printf("Estoque insuficiente! Disponivel: %d\n", disponivel);
                continue;
            }
        }

        if (it != NULL) {
            it->quantidade += qtd;
            it->subtotal   += (Centavos)qtd * p->preco;
        } else {
            if (nItens >= MAX_ITENS) {
                printf("Limite de %d itens diferentes por venda atingido.\n", MAX_ITENS);
                continue;
            }
            itens[nItens].produto    = p;
            itens[nItens].quantidade = qtd;
            itens[nItens].subtotal   = (Centavos)qtd * p->preco;
            nItens++;
        }
        totalVenda += (Centavos)qtd * p->preco;
        mostrarCarrinho(itens, nItens, totalVenda);
    }

    if (nItens == 0) {
        printf("Venda sem itens. Nada foi registrado.\n");
        return;
    }

    mostrarCarrinho(itens, nItens, totalVenda);
    formatarReais(totalVenda, a, sizeof a);
    printf("Total a pagar: %s\n", a);

    for (;;) {
        pago = lerCentavos("Valor recebido em R$ (0 = cancelar venda): ", 0);
        if (pago == 0) {
            printf("Venda cancelada. Nenhum estoque foi alterado.\n");
            return;
        }
        if (pago < totalVenda) {
            formatarReais(totalVenda - pago, b, sizeof b);
            printf("Valor insuficiente! Faltam %s.\n", b);
        } else {
            break;
        }
    }
    troco = pago - totalVenda;

    for (i = 0; i < nItens; i++) {
        itens[i].produto->estoque  -= itens[i].quantidade;
        itens[i].produto->vendidos += itens[i].quantidade;
        itens[i].produto->faturado += itens[i].subtotal;
        unidades += itens[i].quantidade;
    }
    dia->numVendas++;
    dia->itensVendidos += unidades;
    dia->faturamento   += totalVenda;
    dia->totalTroco    += troco;
    if (totalVenda > dia->maiorVenda) dia->maiorVenda = totalVenda;

    printf("\n==================== CUPOM ====================\n");
    printf("Venda numero: %d\n", dia->numVendas);
    for (i = 0; i < nItens; i++) {
        formatarReais(itens[i].produto->preco, a, sizeof a);
        formatarReais(itens[i].subtotal, b, sizeof b);
        printf("%-22.22s %3d x %10s = %11s\n", itens[i].produto->nome,
               itens[i].quantidade, a, b);
    }
    printf("-----------------------------------------------\n");
    formatarReais(totalVenda, a, sizeof a);
    formatarReais(pago, b, sizeof b);
    formatarReais(troco, c, sizeof c);
    printf("TOTAL ........: %s\n", a);
    printf("PAGO .........: %s\n", b);
    printf("TROCO ........: %s\n", c);
    printf("===============================================\n");
    printf("Venda finalizada com sucesso!\n");
}

static void relatorioDia(const Produto *lista, int total, const ResumoDia *dia)
{
    int  i, temVenda = 0, temBaixo = 0;
    const Produto *maisVendido = NULL;
    char v[32];

    pausaTitulo("RELATORIO DO DIA");

    printf("Vendas realizadas ....: %d\n", dia->numVendas);
    printf("Unidades vendidas ....: %d\n", dia->itensVendidos);
    formatarReais(dia->faturamento, v, sizeof v);
    printf("Faturamento total ....: %s\n", v);

    if (dia->numVendas > 0) {
        formatarReais(dia->faturamento / dia->numVendas, v, sizeof v);
        printf("Ticket medio .........: %s\n", v);
        formatarReais(dia->maiorVenda, v, sizeof v);
        printf("Maior venda ..........: %s\n", v);
    } else {
        printf("Ticket medio .........: R$ 0,00\n");
        printf("Maior venda ..........: R$ 0,00\n");
    }
    formatarReais(dia->totalTroco, v, sizeof v);
    printf("Total de troco .......: %s\n", v);

    printf("\n-- Produtos vendidos --\n");
    for (i = 0; i < total; i++) {
        if (lista[i].vendidos > 0) {
            if (!temVenda) {
                printf("%-6s %-26s %6s %14s\n", "COD", "NOME", "QTD", "FATURADO");
                temVenda = 1;
            }
            formatarReais(lista[i].faturado, v, sizeof v);
            printf("%-6d %-26.26s %6d %14s\n", lista[i].codigo, lista[i].nome,
                   lista[i].vendidos, v);
            if (maisVendido == NULL || lista[i].vendidos > maisVendido->vendidos) {
                maisVendido = &lista[i];
            }
        }
    }
    if (!temVenda) {
        printf("Nenhuma venda registrada hoje.\n");
    } else {
        printf("\nProduto mais vendido: %s (%d un.)\n", maisVendido->nome,
               maisVendido->vendidos);
    }

    printf("\n-- Alertas de estoque (<= %d un.) --\n", ESTOQUE_BAIXO);
    for (i = 0; i < total; i++) {
        if (lista[i].estoque <= ESTOQUE_BAIXO) {
            printf("  %s (cod %d): %d un.%s\n", lista[i].nome, lista[i].codigo,
                   lista[i].estoque, lista[i].estoque == 0 ? " [ZERADO]" : "");
            temBaixo = 1;
        }
    }
    if (!temBaixo) printf("  Nenhum produto com estoque baixo.\n");
}

static void mostrarMenu(void)
{
    printf("\n=========== PDV MERCADINHO ===========\n");
    printf(" 1 - Cadastrar produto\n");
    printf(" 2 - Entrada em estoque\n");
    printf(" 3 - Listar produtos\n");
    printf(" 4 - Consultar produto\n");
    printf(" 5 - Nova venda\n");
    printf(" 6 - Relatorio do dia\n");
    printf(" 7 - Carregar produtos de exemplo\n");
    printf(" 0 - Sair\n");
    printf("======================================\n");
}

int main(void)
{
    Produto   produtos[MAX_PRODUTOS];
    int       totalProdutos = 0;
    ResumoDia dia = {0, 0, 0, 0, 0};
    int       opcao;

    do {
        mostrarMenu();
        opcao = lerInt("Escolha uma opcao: ", 0, 7);

        switch (opcao) {
            case 1: cadastrarProduto(produtos, &totalProdutos);        break;
            case 2: entradaEstoque(produtos, totalProdutos);           break;
            case 3: listarProdutos(produtos, totalProdutos);           break;
            case 4: consultarProduto(produtos, totalProdutos);         break;
            case 5: realizarVenda(produtos, totalProdutos, &dia);      break;
            case 6: relatorioDia(produtos, totalProdutos, &dia);       break;
            case 7: carregarExemplos(produtos, &totalProdutos);        break;
            case 0: printf("\nEncerrando o PDV. Ate logo!\n");         break;
        }
    } while (opcao != 0);

    return 0;
}