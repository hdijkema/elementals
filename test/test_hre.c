/*
   This file is part of elementals (http://hoesterholt.github.com/elementals/).
   Copyright 2013, Hans Oesterholt <debian@oesterholt.net>

   Elementals are free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Elementals are distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with elementals.  If not, see <http://www.gnu.org/licenses/>.

   ********************************************************************
*/

#include <elementals.h>  
#include <stdio.h>

#include "../src/regexp.c"

/************************************************************
 * log support
 ***********************************************************/

FILE *log_handle() 
{
  return stderr;
}

int log_this_severity(int level) 
{
  return 1;
}

/************************************************************
 * list declaration
 ***********************************************************/

int* int_copy(int *e) 
{
  int *ee=(int *) mc_malloc(sizeof(int));
  *ee=*e;
  return ee;
}

void int_destroy(int * e) 
{
  mc_free(e);
}

void presult(char* str) 
{
  printf("='%s'",str);
  mc_free(str);
}

void pmatches(hre_matches m) 
{
  int i, n;
  for(i=0, n=hre_matches_count(m);i < n;++i) {
    hre_match_t *mm = hre_matches_get(m,i);
    int bo = hre_match_begin(mm);
    int be = hre_match_end(mm);
    int l = hre_match_len(mm);
    const char *s = hre_match_string(mm);
    printf("m[%d]=%s[%d-%d,%d]\n",i,s,bo,be,l);
  }
  hre_matches_destroy(m);
}


/************************************************************
 * testing
 ***********************************************************/

#define TEST(name,code) \
    printf("%s: ",#name);fflush(stdout); \
    { code } \
    printf(" ok\n");fflush(stdout); 


int main() {

  mc_init();
  
  char* str = "  Hallo, dit is Een 132.2 tekst.  ";
  char* str1 = "Hoi, dit is nog een tekst.  ";
  char* str2 = "  HI, klaar!";
  
  TEST(t1,{ char* r = mc_strdup(str);hre_trim(r);presult(r); });
  TEST(t2,{ char* r = mc_strdup(str);hre_trim(r);hre_trim(r);presult(r); });
  TEST(t3,{ char* r = mc_strdup(str1);hre_trim(r);presult(r); });
  TEST(t4,{ char* r = mc_strdup(str2);hre_trim(r);presult(r); });
  
  TEST(t5,{ char* r = hre_left(str1, 3);presult(r); });
  TEST(t6,{ char* r = hre_left(str1, strlen(str1));presult(r); });
  TEST(t7,{ char* r = hre_left(str1, strlen(str1)+10);presult(r); });
  
  TEST(t8,{ char* r = hre_right(str2, 6);presult(r); });
  TEST(t9,{ char* r = hre_right(str2, strlen(str2));presult(r); });
  TEST(t10,{ char* r = hre_right(str2, strlen(str2)+10);presult(r); });
  
  TEST(t11,{ char* r = hre_substr(str2, 2, -1);presult(r); });
  TEST(t12,{ char* r = hre_substr(str2, 2, 2);presult(r); });
  TEST(t13,{ char* r = hre_substr(str2, 2, 1);presult(r); });
  TEST(t14,{ char* r = hre_substr(str2, 2, 0);presult(r); });
  
  TEST(t15,{ char* r = hre_concat(str, str2);presult(r); });
  TEST(t16,{ char* r = hre_concat3(str, str2, str1);hre_trim(r);presult(r); });
  
  TEST(t40,{ hre_t re = hre_compile("[0-9]+","");
             hre_matches m = hre_match(re, str);
             pmatches(m); 
  });

  TEST(t41,{ hre_t re = hre_compile("[0-9.]+","");
             hre_matches m = hre_match(re, str);
             pmatches(m); 
  });

  TEST(t42,{ hre_t re = hre_compile("([0-9]+)[.]([0-9]+)","");
             hre_matches m = hre_match(re, str);
             pmatches(m); 
  });
  
  TEST(t43,{ hre_t re = hre_compile("e+","i");
             hre_matches m = hre_match(re, str);
             pmatches(m); 
  });
  
  TEST(t44,{ hre_t re = hre_compile("e+","i");
             hre_matches m = hre_match_all(re, str);
             pmatches(m); 
  });

  TEST(t45,{ hre_t re = hre_compile("[0-9]","i");
             hre_matches m = hre_match_all(re, str);
             pmatches(m); 
  });

  TEST(t46,{ char* s = hre_substr(str, 27, 1);
             presult(s);
  });

  TEST(t47,{ hre_t re = hre_compile("^\\s+","");
             char* r = hre_replace(re, str, "");
             presult(r);
  });

  TEST(t48,{ hre_t re = hre_compile("\\s*$","");
             char* r = hre_replace(re, str, "");
             presult(r);
  });

  TEST(t49,{ hre_t re = hre_compile("^\\s*hallo","i");
             char* r = hre_replace(re, str, "GOEDEMORGEN!");
             presult(r);
  });

  TEST(t50,{ hre_t re = hre_compile("[0-9.]+","i");
             char* r = hre_replace(re, str, "GOEDEMORGEN!");
             hre_trim(r);
             presult(r);
  });

  TEST(t51,{ hre_t re = hre_compile("[0-9.]+","i");
             printf("has match = %d\n", hre_has_match(re, str ));
  });
  
  TEST(t52,{ hre_t re = hre_compile("e","i");
             char* r = hre_replace_all(re, str, "oo");
             hre_trim(r);
             hre_t re1 = hre_compile("[0-9]+","");
             char* r1 = hre_replace_all(re1, str, "T"); 
             hre_trim(r1);
             presult(r);
             printf("\n");
             presult(r1);
  });
  
  return 0;
}
