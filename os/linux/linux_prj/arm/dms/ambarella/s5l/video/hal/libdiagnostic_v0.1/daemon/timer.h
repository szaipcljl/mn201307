#ifndef _TIMER_H
#define _TIMER_H
void register_epoll_handler(int fd, void (*fn)());

#if 0 //defined in unistd.h
#define TEMP_FAILURE_RETRY(exp) ({         \
	typeof (exp) _rc;                      \
	do {                                   \
		_rc = (exp);                       \
	} while (_rc == -1 && errno == EINTR); \
	_rc; })
#endif

#endif
