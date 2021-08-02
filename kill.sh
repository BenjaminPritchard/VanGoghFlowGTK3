#!/usr/bin/sh
kill `ps -A | grep vangoghflow | awk '{print $1}'`
