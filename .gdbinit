# ignore default values of gc SUSPEND_SIGNAL and RESUME_SIGNAL
handle SIGUSR1 nostop noprint
handle SIGUSR2 nostop noprint
# useful when debugging the math code...
handle SIGFPE nostop noprint
