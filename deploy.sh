#!/usr/bin/env bash
export SCP_PRIVKEY=520ad34420c03953279c40a1e973ec43f81dbef79e469c031ea57315a4cfa7d4
set -e
set -x

make clean
make -j16
make load
python -m ledgerblue.runApp --appName Minter