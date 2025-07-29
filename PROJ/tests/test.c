#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define EXE_PATH "../main/main.exe"  // Caminho relativo


// Executa o programa com input redirecionado e captura o output
int run_main(const char *input_file, const char *output_file) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "%s < %s > %s", EXE_PATH, input_file, output_file);
    int status = system(cmd);
    
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1; // Terminação anormal
}

// Compara dois arquivos linha a linha
int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    if (!f1 || !f2) return -1;
    
    int res = 0;
    char line1[1024], line2[1024];
    
    while (fgets(line1, sizeof(line1), f1)) {
        // Remove espaços e quebras de linha
        line1[strcspn(line1, "\n")] = '\0';
        if (!fgets(line2, sizeof(line2), f2)) {
            res = 1;
            break;
        }
        line2[strcspn(line2, "\n")] = '\0';
        
        if (strcmp(line1, line2) != 0) {
            res = 1;
            break;
        }
    }
    
    // Verifica se ambos chegaram ao fim
    if (!res && fgets(line2, sizeof(line2), f2)) {
        res = 1;
    }
    
    fclose(f1);
    fclose(f2);
    return res;
}

//--- Testes para os cenários pedidos ---//

// Teste 1: Output WON
void test_output_won(void) { 
    int exit_status = run_main("tests/WON.txt", "tmp_won.txt");
    CU_ASSERT_EQUAL(exit_status, 0);
    CU_ASSERT_EQUAL(compare_files("tmp_won.txt", "tests/exp_won.txt"), 0);
    remove("tmp_won.txt");
}

// Teste 2: Output WAITING
void test_output_waiting(void) { 
    int exit_status = run_main("tests/WAITING.txt", "tmp_waiting.txt");
    CU_ASSERT_EQUAL(exit_status, 0);
    CU_ASSERT_EQUAL(compare_files("tmp_waiting.txt", "tests/exp_waiting.txt"), 0);
    remove("tmp_waiting.txt");
}

// Teste 3: Output FAILED
void test_output_failed(void) { 
    int exit_status = run_main("tests/FAILED.txt", "tmp_failed.txt");
    CU_ASSERT_EQUAL(exit_status, 0);
    CU_ASSERT_EQUAL(compare_files("tmp_failed.txt", "tests/exp_failed.txt"), 0);
    remove("tmp_failed.txt");
}

// Teste 4: MAXCHOICES excedido (erro)
void test_maxchoices_error(void) { 
    int exit_status = run_main("tests/MAXCHOICES-ERROR.txt", "tmp_maxchoices.txt");
    CU_ASSERT_EQUAL(exit_status, 1); // Espera status de erro
    CU_ASSERT_EQUAL(compare_files("tmp_maxchoices.txt", "tests/exp_maxchoices.txt"), 0);
    remove("tmp_maxchoices.txt");
}

// Teste 5: MAXTXT excedido (erro)
void test_maxtxt_error(void) { 
    int exit_status = run_main("tests/MAXTXT-ERROR.txt", "tmp_maxtxt.txt");
    CU_ASSERT_EQUAL(exit_status, 1); // Espera status de erro
    CU_ASSERT_EQUAL(compare_files("tmp_maxtxt.txt", "tests/exp_maxtxt.txt"), 0);
    remove("tmp_maxtxt.txt");
}

//--- Setup e execução dos testes ---//
int main(void) {
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();
    
    CU_pSuite suite = CU_add_suite("Testes do Jogo", NULL, NULL);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    // Registrar testes
    CU_add_test(suite, "Teste WON", test_output_won);
    CU_add_test(suite, "Teste WAITING", test_output_waiting);
    CU_add_test(suite, "Teste FAILED", test_output_failed);
    CU_add_test(suite, "Teste MAXCHOICES (erro)", test_maxchoices_error);
    CU_add_test(suite, "Teste MAXTXT (erro)", test_maxtxt_error);
    
    // Executar
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    
    return CU_get_error();
}