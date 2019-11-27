#!/usr/bin/env bash

PYTHON_BIN=$(which python | tr -d "\n")
PIP_BIN=$(which pip | tr -d "\n")

if [ "${PYTHON_BIN}" == "" ]
then
	echo "Python not found on your system"
	exit 255
fi

if [ "${PIP_BIN}" == "" ]
then
	echo "Python PIP not found on your system"
	exit 255
fi

HAVE_LEDGERBLUE=$(${PYTHON_BIN} -c "import ledgerblue")
if [ "${HAVE_LEDGERBLUE}" != "" ]
then
	${PIP_BIN} install image
	${PIP_BIN} install ledgerblue
fi


${PYTHON_BIN} -m ledgerblue.loadApp \
--apdu \
--appFlags 0x40 \
--path "44'/60'" \
--curve secp256k1 \
--tlv \
--targetId 0x31100004 \
--delete \
--fileName app.hex \
--appName Minter \
--appVersion 0.1.0 \
--dataSize 0x0000000000000000 \
--icon 0100000000ffffff00ffffffffffffe7ebcfe99fec37e667e7e7e7e7e7e7e7e7e7e7e7ffffffffffff

