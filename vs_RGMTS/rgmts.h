#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>

//#define WIN32


#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif // WIN32



#include <time.h>

#include <string.h>
#include <math.h>

#include "basic_define.h"
#include "wrapper_function.h"
#include "matrix_and_s.h"
#include "DFMRS.h"
#include "Get_Keys.h"
#include "my_index.h"
#include "Server_Init.h"

#include "results.h"
#include "search.h"

#include "Get_Keys.h"
#include "Input_Words.h"
#include "Trapdoor_Construction.h"
#include "Init_keys.h"

