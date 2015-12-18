#!/bin/bash

ftp -n -v fritz.nas << EOL
ascii
user user 1234
prompt
cd tobias/ti_nspire
put nnimmul.tns
bye
EOL
