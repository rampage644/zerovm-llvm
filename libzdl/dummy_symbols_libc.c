#include <signal.h>
#include <string.h>
#include <iconv.h>
#include <locale.h>
#include <stropts.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <assert.h>
#include <complex.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
#include <fcntl.h>
#include <sys/stat.h>

void dummy() {
  int p = 0;
  p = (int)&iconv_open;
  p = (int)&iconv;
  p = (int)&iconv_close;
  p = (int)&setlocale;
  p = (int)&isalnum;
  p = (int)&isalpha;
  p = (int)&iscntrl;
  p = (int)&isdigit;
  p = (int)&isgraph;
  p = (int)&islower;
  p = (int)&isprint;
  p = (int)&ispunct;
  p = (int)&isspace;
  p = (int)&isupper;
  p = (int)&isxdigit;
  p = (int)&tolower;
  p = (int)&toupper;
  p = (int)&isascii;
  p = (int)&toascii;
  p = (int)&setjmp;
  p = (int)&raise;
  p = (int)&killpg;
  p = (int)&sigpending;
  p = (int)&siginterrupt;
  p = (int)&sigemptyset;
  p = (int)&sigfillset;
  p = (int)&sigaddset;
  p = (int)&sigdelset;
  p = (int)&sigismember;
  p = (int)&atof;
  p = (int)&atoi;
  p = (int)&atol;
  p = (int)&atoll;
  p = (int)&abort;
  p = (int)&bsearch;
  p = (int)&qsort;
  p = (int)&getenv;
  p = (int)&putenv;
  p = (int)&exit;
  p = (int)&atexit;
  p = (int)&abs;
  p = (int)&labs;
  p = (int)&div;
  p = (int)&ldiv;
  p = (int)&mblen;
  p = (int)&mbstowcs;
  p = (int)&mbtowc;
  p = (int)&wcstombs;
  p = (int)&wctomb;
  p = (int)&rand;
  p = (int)&rand_r;
  p = (int)&drand48;
  p = (int)&erand48;
  p = (int)&lrand48;
  p = (int)&nrand48;
  p = (int)&mrand48;
  p = (int)&jrand48;
  p = (int)&srand48;
  p = (int)&seed48;
  p = (int)&lcong48;
  p = (int)&drand48_r;
  p = (int)&lrand48_r;
  p = (int)&mrand48_r;
  p = (int)&strtol;
  p = (int)&strtoul;
  p = (int)&strtoll;
  p = (int)&strtoull;
  p = (int)&a64l;
  p = (int)&l64a;
  p = (int)&rpmatch;
  p = (int)&strtoimax;
  p = (int)&strtoumax;
  p = (int)&wcstoimax;
  p = (int)&wcstoumax;
  p = (int)&ctermid;
  p = (int)&vfprintf;
  p = (int)&vprintf;
  p = (int)&fprintf;
  p = (int)&printf;
  p = (int)&sprintf;
  p = (int)&fscanf;
  p = (int)&scanf;
  p = (int)&sscanf;
  p = (int)&perror;
  p = (int)&psignal;
  p = (int)&tmpnam;
  p = (int)&tmpnam_r;
  p = (int)&tempnam;
  p = (int)&getw;
  p = (int)&putw;
  p = (int)&remove;
  p = (int)&rename;
  p = (int)&fputwc;
  p = (int)&getwchar;
  p = (int)&fgetws;
  p = (int)&fputws;
  p = (int)&ungetwc;
  p = (int)&putwc;
  p = (int)&putwchar;
  p = (int)&putchar;
  p = (int)&clearerr;
  p = (int)&fileno;
  p = (int)&fputc;
  p = (int)&freopen;
  p = (int)&fseek;
  p = (int)&getchar;
  p = (int)&rewind;
  p = (int)&setbuf;
  p = (int)&setlinebuf;
  p = (int)&fseeko;
  p = (int)&ftello;
  p = (int)&clearerr_unlocked;
  p = (int)&feof_unlocked;
  p = (int)&ferror_unlocked;
  p = (int)&fputc_unlocked;
  p = (int)&getchar_unlocked;
  p = (int)&fflush_unlocked;
  p = (int)&putc_unlocked;
  p = (int)&fread_unlocked;
  p = (int)&fwrite_unlocked;
  p = (int)&strcat;
  p = (int)&strchr;
  p = (int)&strcmp;
  p = (int)&strcoll;
  p = (int)&strcpy;
  p = (int)&strcspn;
  p = (int)&strerror;
  p = (int)&strlen;
  p = (int)&strncat;
  p = (int)&strncmp;
  p = (int)&strncpy;
  p = (int)&strrchr;
  p = (int)&strpbrk;
  p = (int)&strspn;
  p = (int)&strstr;
  p = (int)&strtok;
  p = (int)&strxfrm;
  p = (int)&memcmp;
  p = (int)&memmove;
  p = (int)&memset;
  p = (int)&bcopy;
  p = (int)&memcpy;
  p = (int)&wcschr;
  p = (int)&wcscmp;
  p = (int)&wcscpy;
  p = (int)&wcscspn;
  p = (int)&wcsncat;
  p = (int)&wcsncmp;
  p = (int)&wcspbrk;
  p = (int)&wcsrchr;
  p = (int)&wcsspn;
  p = (int)&wcstok;
  p = (int)&wcsstr;
  p = (int)&wmemchr;
  p = (int)&wmemcmp;
  p = (int)&wmemset;
  p = (int)&wctob;
  p = (int)&wcstol;
  p = (int)&wcstoul;
  p = (int)&wcscoll;
  p = (int)&wcsxfrm;
  p = (int)&asctime;
  p = (int)&ctime;
  p = (int)&ctime_r;
  p = (int)&difftime;
  p = (int)&gmtime;
  p = (int)&localtime;
  p = (int)&mktime;
  p = (int)&time;
  p = (int)&stime;
  p = (int)&dysize;
  p = (int)&timegm;
  p = (int)&strftime;
  p = (int)&wcsftime;
  p = (int)&lockf;
  p = (int)&posix_fadvise;
  p = (int)&posix_fallocate;
  p = (int)&mktemp;
  p = (int)&mkstemp;
  p = (int)&mkdtemp;
  p = (int)&ecvt;
  p = (int)&fcvt;
  p = (int)&gcvt;
  p = (int)&ecvt_r;
  p = (int)&fcvt_r;
  p = (int)&qecvt;
  p = (int)&qfcvt;
  p = (int)&qgcvt;
  p = (int)&qecvt_r;
  p = (int)&qfcvt_r;
  p = (int)&getloadavg;
  p = (int)&gnu_dev_major;
  p = (int)&gnu_dev_makedev;
  p = (int)&gnu_dev_minor;
  p = (int)&towlower;
  p = (int)&towupper;
  p = (int)&isastream;
  p = (int)&getmsg;
  p = (int)&getpmsg;
  p = (int)&putmsg;
  p = (int)&putpmsg;
  p = (int)&fattach;
  p = (int)&fdetach;
 }

