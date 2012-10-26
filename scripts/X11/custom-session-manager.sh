#!/bin/bash
unclutter -idle 1 -root &
URL="http://127.0.0.1/test.html"
exec chromium-browser %u --start-maximized --kiosk -incognito "$URL"
