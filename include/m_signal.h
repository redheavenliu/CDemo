#ifndef M_SIGNAL_H
#define M_SIGNAL_H

#include "m_stdio.h"

#include <signal.h>


typedef	void	Sigfunc(int);	/* for signal handlers */


Sigfunc *
signal(int signo, Sigfunc *func);

#endif /* M_SIGNAL_H */