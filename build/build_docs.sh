#!/bin/bash
cd documentation
if [[ $QUITE_DOCS = "YES" ]]
then
  PROJECT_NUMBER=("commit " $(git rev-parse --short HEAD)) QUIET=${QUITE_DOCS} doxygen > doc_build_logs.log
else
  PROJECT_NUMBER=("commit " $(git rev-parse --short HEAD)) QUIET=${QUITE_DOCS} doxygen | tee doc_build_logs.log
fi
if grep -q 'Exiting...' doc_build_logs.log
then
  cd ..
  exit 64
else
  cd ..
fi
