#!/bin/bash

SUBSTR=s~\${BASE_URL}~$BASE_URL~g
sed -i $SUBSTR /var/www/html/conf/nginx/nginx-site.conf
/start.sh