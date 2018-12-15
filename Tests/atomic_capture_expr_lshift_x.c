#include "acc_testsuite.h"

bool is_possible(unsigned int* a, unsigned int* b, int length, int prev){
    if (length == 0){
        return true;
    }
    unsigned int *passed_a = (unsigned int *)malloc((length - 1) * sizeof(int));
    unsigned int *passed_b = (unsigned int *)malloc((length - 1) * sizeof(int));
    for (int x = 0; x < length; ++x){
        if (b[x] == (a[x]<<prev)){
            for (int y = 0; y < x; ++y){
                passed_a[y] = a[y];
                passed_b[y] = b[y];
            }
            for (int y = x + 1; y < length; ++y){
                passed_a[y - 1] = a[y];
                passed_b[y - 1] = b[y];
            }
            if (is_possible(passed_a, passed_b, length - 1, b[x])){
                free(passed_a);
                free(passed_b);
                return true;
            }
        }
    }
    free(passed_a);
    free(passed_b);
    return false;
}

int test(){
    int err = 0;
    srand(time(NULL));
    unsigned int *a = (unsigned int *)malloc(3 * n * sizeof(int));
    unsigned int *b = (unsigned int *)malloc(n * sizeof(int));
    unsigned int *c = (unsigned int *)malloc(3 * n * sizeof(int));

    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 3; ++y){
            a[x * 3 + y] = (int) (rand() / (unsigned int) (RAND_MAX / 4));
        }
        b[x] = 0;
    }

    #pragma acc data copyin(a[0:3 * n]) copy(b[0:n]) copyout(c[0:3*n])
    #pragma acc parallel
    {
        #pragma acc loop
        for (int x = 0; x < n; ++x){
            #pragma acc loop
            for (int y = 0; y < 3; ++y){
                #pragma acc atomic capture
                    c[x * 3 + y] = b[x] = a[x * 3 + y] << b[x];
            }
        }
    }

    int orders[18] = {1, 2, 0, 1, 0, 2, 2, 1, 0, 2, 0, 1, 0, 1, 2, 0, 2, 1};
    int result;
    for (int x = 0; x < n; ++x){
        for (int y = 0; y < 6; ++y){
            result = 0;
            for (int z = 0; z < 3; ++z){
                result = a[x * 3 + orders[y * 3 + z]] << result;
            }
            if (result == b[x]){
                break;
            }
        }
        if (result != b[x]){
            err += 1;
        }
    }

    for (int x = 0; x < n; ++x){
        if (!(is_possible(&(a[x*3]), &(c[x*3]), 3, 0))){
            err += 1;
        }
    }

    free(a);
    free(b);
    free(c);
    return err;
}


int main()
{
  int i;			/* Loop index */
  int result;		/* return value of the program */
  int failed=0; 		/* Number of failed tests */
  int success=0;		/* number of succeeded tests */
  static FILE * logFile;	/* pointer onto the logfile */
  static const char * logFileName = "test_acc_lib_acc_wait.log";	/* name of the logfile */


  /* Open a new Logfile or overwrite the existing one. */
  logFile = fopen(logFileName,"w+");

  printf("######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  printf("## Repetitions: %3d                       ####\n",REPETITIONS);
  printf("## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  printf("##############################################\n");
  printf("Testing test_acc_lib_acc_wait\n\n");

  fprintf(logFile,"######## OpenACC Validation Suite V %s #####\n", ACCTS_VERSION );
  fprintf(logFile,"## Repetitions: %3d                       ####\n",REPETITIONS);
  fprintf(logFile,"## Array Size : %.2f MB                 ####\n",ARRAYSIZE * ARRAYSIZE/1e6);
  fprintf(logFile,"##############################################\n");
  fprintf(logFile,"Testing test_acc_lib_acc_wait\n\n");

  for ( i = 0; i < REPETITIONS; i++ ) {
    fprintf (logFile, "\n\n%d. run of test_acc_lib_acc_wait out of %d\n\n",i+1,REPETITIONS);
    if (test() == 0) {
      fprintf(logFile,"Test successful.\n");
      success++;
    } else {
      fprintf(logFile,"Error: Test failed.\n");
      printf("Error: Test failed.\n");
      failed++;
    }
  }

  if(failed==0) {
    fprintf(logFile,"\nDirective worked without errors.\n");
    printf("Directive worked without errors.\n");
    result=0;
  } else {
    fprintf(logFile,"\nDirective failed the test %i times out of %i. %i were successful\n",failed,REPETITIONS,success);
    printf("Directive failed the test %i times out of %i.\n%i test(s) were successful\n",failed,REPETITIONS,success);
    result = (int) (((double) failed / (double) REPETITIONS ) * 100 );
  }
  printf ("Result: %i\n", result);
  return result;
}