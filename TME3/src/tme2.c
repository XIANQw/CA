#include <stdio.h>
#include <stdlib.h>

#define NREG 8
#define V(x) regs[x]
#define LIMIT 4294967296
#define NSIZE 100000
#define EXCEPTION -5
#define SUCCES 99
#define STOP 100
#define SIZE 1024    

typedef unsigned int uint;

int NTAB = 2;
uint tableau0[NSIZE];
uint ** tableaux;
size_t * size_tab;
uint regs[NREG];
uint cpt_code, cpt_tab, code_index;
FILE * fp;

int readbyte(){
    uint code;
    uint res = 0;
    for(int i = 0; i < 4; i++){
        code = fgetc(fp);
        //printf("%x ",code);
        res += code << 8*(3-i);
        if(code == EOF) return EOF;
    }
    //printf("\n");
    return res;
}

void init(char * filename){
    fp = fopen(filename, "rb");
    tableaux = (uint **)malloc(sizeof(uint *) * NTAB);
    size_tab = (size_t *) malloc(sizeof(size_t) * NTAB);
    cpt_tab = 1;
    uint code;
    for(int i = 0; i < NSIZE; i++) {
        code = readbyte();
        if(code == EOF) break;
        tableau0[i] = code;
        cpt_code++;
    }
    fclose(fp);
}

void mouvement(uint a, uint b, uint c){
    if(V(c)){
        V(a) = V(b);
        //printf("regs[%d] = regs[%d]\n", a, b);
    }
}

void indice(uint a, uint b, uint c){
    uint offset = V(c), id = V(b);
    //printf("id :%d, offset: %d\n", id, offset);
    if(id != 0 && (id >= NTAB || offset >= size_tab[id])){
        //printf("Exception: index out of range. id:%d >= %d or offset:%d >= %ld\n", id, NTAB, offset, size_tab[id]);
        return ;
    }
    if(id == 0 && offset > NSIZE) return ;
    if(id == 0) V(a) = tableau0[offset];
    else V(a) = tableaux[id][offset];
    //printf("regs[%d] = tableaux[%d][%d]\n", a, id, offset);
}

void set_tab(uint a, uint b, uint c){
    uint id = V(a), offset = V(b);
    if(id != 0 && (id >= NTAB || offset >= size_tab[id])){
        //printf("Exception: index out of range. id:%d >= %d or offset:%d >= %ld\n", id, NTAB, offset, size_tab[id]);
        return ;
    }
    if(id == 0 && offset > NSIZE) return ;
    if(id == 0) tableau0[offset] = V(c); 
    else tableaux[id][offset] = V(c);
    //printf("tableaux[%d][%d] = regs[%d] = %d\n", id, offset, c, V(c));
}

void addition(uint a, uint b, uint c){
    V(a) = (V(b) + V(c)) % LIMIT;
    //printf("regs[%d] = regs[%d] + regs[%d] = %x + %x = %x\n", a, b, c, V(b), V(c), V(a));
}

void mult(uint a, uint b, uint c){
    V(a) = (V(b) * V(c)) % LIMIT;
    //printf("regs[%d] = regs[%d] * regs[%d] = %x * %x = %x\n", a, b, c, V(b), V(c), V(a));
}

void division(uint a, uint b, uint c){
    if(!V(c)) return ;
    V(a) = (V(b) / V(c));
}

void nand(uint a, uint b, uint c){
    V(a) = ~(V(b) & V(c));
}

void redimension(){
    int old_ntab = NTAB;
    NTAB *= 2;
    uint ** cache = (uint **)malloc(sizeof(uint *)*NTAB);
    for(int i=0; i<old_ntab; i++){
        cache[i] = (uint *)malloc(sizeof(uint)*size_tab[i]);
        for(int j=0; j<size_tab[i]; j++){
            cache[i][j] = tableaux[i][j];
        }
        free(tableaux[i]);
    }
    free(tableaux);
    tableaux = cache;
    size_t * cache_size_tab = (size_t *)malloc(sizeof(size_t) * NTAB);
    for(int i=0; i<old_ntab; i++){
        cache_size_tab[i] = size_tab[i];
    }
    for(int i=old_ntab; i<NTAB; i++){
        cache_size_tab[i] = 0;
    }
    free(size_tab);
    size_tab = cache_size_tab;
}

void alloc(uint b, uint c){
    if(cpt_tab == NTAB){ 
        //printf("redimmensionnement\n");
        redimension();
    }
    uint size = V(c);
    if(size == 0) return ;
    int * newtab;
    if((newtab = (uint *) malloc(sizeof(uint) * size)) == NULL){
        //printf("Exception : allocation error\n");
        return ;
    }
    for(int i = 0; i<size; i++){
        newtab[i] = 0;
    }
    uint id = 1;
    while(size_tab[id]) id = id + 1;
    tableaux[id] = newtab;
    //printf("tableaux[%d] = malloc(sizeof(int) * %d", id, size);
    cpt_tab++;
    V(b) = id;
    size_tab[id] = size;
}

void abandon(uint c){
    if(V(c) == 0) return ;
    uint id = V(c);
    //printf("free(table[%d])\n", id);
    if(!cpt_tab || !size_tab[id]){
        //printf("Exception: double free a table\n");
        return ;
    }
    cpt_tab--;
    size_tab[id] = 0;
    free(tableaux[id]);
    tableaux[id] = NULL;
}

void console(uint c){
    putchar(V(c)); 
}

void input(uint c){
    V(c) = getchar();
}

void charger_prog(uint b, uint c){
    int id;
    if(V(b) == 0){
        code_index = V(c) - 1;
        //printf("charge prog table[0] to table[0], indice %d to %d\n", code_index, V(c));
        return ;    
    }
    id = V(b) - 1;
    //printf("charge prog table[%d] to table[0], indice %d to %d\n", id, code_index, V(c));
    if(!size_tab[id]){
        //printf("Exception : access tableaux[%d] without allocation\n", id);
        return ;
    }
    for(int i = 0; i<size_tab[id]; i++) tableau0[i] = tableaux[id][i];
    cpt_code = size_tab[id];
    code_index = V(c) - 1;
}

void affect(uint code){
    uint a = (code >> 25) & 0b111;
    regs[a] = code & 0b1111111111111111111111111;
    return;
}

int calcul(uint code){
    uint tmp = code, a, b, c;
    c = tmp & 7; tmp = tmp >> 3;
    b = tmp & 7; tmp = tmp >> 3;
    a = tmp & 7; tmp = tmp >> 3;
    uint num = code >> 28;
    switch(num){
        case 0:  mouvement(a, b, c);return SUCCES;
        case 1:  indice(a, b, c);return SUCCES;
        case 2:  set_tab(a, b, c);return SUCCES;
        case 3:  addition(a, b, c);return SUCCES;
        case 4:  mult(a, b, c);return SUCCES;;
        case 5:  division(a, b, c);return SUCCES;;
        case 6:  nand(a, b, c);return SUCCES;;
        case 7:  return STOP;
        case 8:  alloc(b, c);return SUCCES;;            
        case 9:  abandon(c);return SUCCES;;
        case 10:  console(c);return SUCCES;;
        case 11:  input(c);return SUCCES;;
        case 12:  charger_prog(b, c);return SUCCES;; 
        case 13:  affect(code);return SUCCES;;
    }
    
}

int demarrer(){
    code_index = 0;
    int res;
    while(code_index < cpt_code){
        uint code = tableau0[code_index];
        //printf("indice: %d, ", code_index);
        //printf("code: %x, M%d\n", code, code >> 28);
        res = calcul(code);
        // for(int i=0; i < NREG; i++){
        //     printf("%x ", regs[i]);
        // }
        if(res == STOP) return STOP;        
        //printf("\n");
        code_index = code_index + 1;
    }
    return SUCCES;
}




int main(){
    init("data/sandmark.umz");
    demarrer();
    return 0;
}