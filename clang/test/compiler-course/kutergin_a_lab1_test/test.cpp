// RUN: %clang_cc1 -load %llvmshlibdir/VarScopeAnalysis_Kutergin_Anton_FIIT1_ClangAST.so -plugin var-scope-stats %s 2>&1 | FileCheck %s

extern unsigned externValue;
void foo(int a, double b);

// CHECK: --- Unit Statistics (Kutergin Anton) ---
// CHECK-NEXT: Total Declarations : 19
// CHECK-NEXT: Global scope : 3
// CHECK-NEXT: Static storage : 9
// CHECK-NEXT: Local variables : 3
// CHECK-NEXT: Function parameters: 4

static int g_init_flag = 1;          
static double g_scale_factor = 0.5;   
static char g_system_code = 'K';      

static int v1, v2;                    

namespace Core {
    extern float weight_limit;        
    static int internal_id;           
}

namespace Utils {
    extern float weight_limit;        
    static int internal_id;           
}

namespace Core {
    float weight_limit = 100.0f;      
}

namespace {
    extern int secret_key;            
    static int local_buffer;          
}
namespace {
    int secret_key;                  
}

extern int external_linkage;          
int external_linkage = 42;            

static void run_logic(int mode, float threshold) { 
    int status_code = mode + 10;                   
}

int main(int argc, char** argv) {      
    int result = 0;                    
    static int offset = 5;   
    int iteration_index = 0;           
    return result;
}