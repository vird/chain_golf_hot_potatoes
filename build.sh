#!/usr/bin/env bash

SOURCE_FILE=main.cpp
OUTPUT_FILE=node.out

BOOST_LIBRARIES="boost_chrono boost_context boost_filesystem boost_iostreams boost_locale boost_program_options boost_regex boost_serialization boost_system boost_thread boost_timer boost_wave boost_date_time boost_graph boost_random boost_exception"
LIBRARIES="${BOOST_LIBRARIES} pthread jsoncpp microhttpd jsonrpccpp-common jsonrpccpp-server"
LINK_RESULT=""
for each in ${LIBRARIES}; do
  LINK_RESULT="${LINK_RESULT} -l${each}"
done

COMMAND="g++-9 ${SOURCE_FILE} -o ${OUTPUT_FILE} -pedantic -O2 -std=c++2a -fPIC ${LINK_RESULT}"
echo "execute command: ${COMMAND}"
bash -c "${COMMAND}"
