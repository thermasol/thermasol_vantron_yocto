#!/bin/sh

EVENT=${1:-short-press}

TIMEOUT=6 # s
PIDFILE="/tmp/$(basename $0).pid"
LOCKFILE=/tmp/.restore_key
LOGFILE=/var/log/factory.log

factory_mode()
{
	echo "$(date) vt restore" >> ${LOGFILE}
	if [ ! -f $LOCKFILE ]; then
		touch $LOCKFILE
		run_factory
		rm -f $LOCKFILE
		echo "$(date) vt restore complete" >> ${LOGFILE}
	fi
}

logger -t $(basename $0) "[$$]: Received power key event: $@..."

case "$EVENT" in
	press)
		# Lock it
		exec 3<$0
		flock -x 3

		start-stop-daemon -K -q -p $PIDFILE || true
		start-stop-daemon -S -q -b -m -p $PIDFILE -x /bin/sh -- \
			-c "sleep $TIMEOUT; $0 long-press"

		# Unlock
		flock -u 3
		;;
	release)
		# Avoid race with press event
		sleep .5
		echo "$(date) vt release key" >> ${LOGFILE}
		if [ ! -f $LOCKFILE ]; then
			start-stop-daemon -K -q -p $PIDFILE
		fi
		;;
	short-press)
		;;
	long-press)
		factory_mode
		;;
esac