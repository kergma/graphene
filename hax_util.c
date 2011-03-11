#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "hax_util.h"
#include "xlockmore.h"

void error_exit(const char *message)
{
	fprintf(stderr, "%s: %s\n", progname, message);
	exit(1);
}


