#!/bin/sh
if [ "$EUID" -ne 0 ]
  then echo "Ejecute el script como root"
  exit
else
  cp programabanda.service /etc/systemd/system/programabanda.service
  cp servidorweb.service /etc/systemd/system/servidorweb.service
  echo "Archivo enviado a /etc/systemd/system/servidorweb.service"
  echo "Archivo enviado a /etc/systemd/system/programabanda.service "
fi
