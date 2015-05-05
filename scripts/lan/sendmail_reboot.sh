#!/bin/bash
. ~/config.ini
echo "subject : reboot " | /usr/bin/msmtp $MAIL
