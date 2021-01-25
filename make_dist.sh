#!/usr/bin/env bash
mkdir -p dist
cp bin/* dist/
PYTHON_BIN=$(which python3 | tr -d "\n")

INST_FILE="dist/install.sh"
VERS=`cat version | tr -d "\n"`
ICONHEX=$(${PYTHON_BIN} ${PWD}/libs/sdk/icon.py nanos_app_minter.gif hexbitmaponly 2>/dev/null)

# for 1.5.x
# DATASIZE=$(cat debug/app.map | grep _nvram_data_size | tr -s ' ' | cut -f2 -d' ')
# since 1.6.x
DATASIZE=$((0x`cat debug/app.map |grep _envram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'` - 0x`cat debug/app.map |grep _nvram_data | tr -s ' ' | cut -f2 -d' '|cut -f2 -d'x'`))

touch ${INST_FILE}

cat << INSTALL > ${INST_FILE}
#!/usr/bin/env bash

PYTHON_BIN=\$(which python3 | tr -d "\n")
PIP_BIN=\$(which pip3 | tr -d "\n")

if [ "\${PYTHON_BIN}" == "" ]
then
	echo "Python not found on your system"
	exit 255
fi

if [ "\${PIP_BIN}" == "" ]
then
	echo "Python PIP not found on your system"
	exit 255
fi

HAVE_LEDGERBLUE=\$(\${PYTHON_BIN} -c "import ledgerblue")
if [ "\${HAVE_LEDGERBLUE}" != "" ]
then
	\${PIP_BIN} install image
	\${PIP_BIN} install ledgerblue
fi


\${PYTHON_BIN} -m ledgerblue.loadApp \\
--apdu \\
--appFlags 0x40 \\
--path "44'/60'" \\
--curve secp256k1 \\
--tlv \\
--targetId 0x31100004 \\
--delete \\
--fileName app.hex \\
--appName Minter \\
--appVersion ${VERS} \\
--dataSize ${DATASIZE} \\
--icon ${ICONHEX}

INSTALL


