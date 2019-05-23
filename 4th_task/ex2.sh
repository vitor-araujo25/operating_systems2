#!/bin/sh

if [ $# -eq 0 ];then
	echo "Digite um caminho para testar"
	exit -1
fi
if [ -e $1 ]; then
	if [ -d $1 ]; then
		echo "É um diretório"
	fi
	if [ -f $1 ]; then
		echo "É um arquivo"
	fi
else
	echo "O caminho não existe"
fi	
