
#include "mrThread.h"

static bool mrl_abort  = false;
static bool is_running = false;
static bool socket_run = false;
MR_THREAD_RETURN _mr_socket_check_cb( void* v );
