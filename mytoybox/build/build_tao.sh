
if [ -f ~/Downloads/ACE+TAO-6.3.1.tar.bz2 ];
then
	echo "File ACE+TAO-6.3.1.tar.bz2 exists."
else
		echo "ACE+TAO-6.3.1.tar.bz2 does NOT exists."
    wget ftp://download.dre.vanderbilt.edu/previous_versions/ACE+TAO-6.3.1.tar.bz2 -P ~/Downloads
fi

if [ ! -d ACE_wrappers ]; then
    tar xf ~/Downloads/ACE+TAO-6.3.1.tar.bz2
if (($? > 0)); then    
    echo "Please check ~/Downloads/ACE+TAO-6.3.1.tar.bz2"
    exit $?
fi
fi



export ACE_ROOT=$PWD/ACE_wrappers

echo '#include "ace/config-linux.h"' > $ACE_ROOT/ace/config.h
if (($? > 0)); then    
    echo "failed to create $ACE_ROOT/ace/config.h"
    exit $?
fi

rm $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo 'shared_libs=1' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo 'static_libs=0' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo 'static_libs_only=0' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
#echo 'no_hidden_visibility=1' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo 'debug=0' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
#echo 'ACE_COMPONENTS=FOR_TAO' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo 'include $(ACE_ROOT)/include/makeinclude/platform_linux.GNU' >> $ACE_ROOT/include/makeinclude/platform_macros.GNU


if [ -L $0 ] ; then
    SCRIPTDIR=$(dirname $(readlink -f $0)) ;
else
    SCRIPTDIR=$(dirname $0) ;
fi ;

LOGFILE=${SCRIPTDIR}/build-$(date +%Y%m%d%H%M).txt

cd $ACE_ROOT/ace
time make 2>&1 | tee -a ${LOGFILE}

cd $ACE_ROOT/apps/gperf/src ; time make  2>&1 | tee -a ${LOGFILE}
export TAO_ROOT=$ACE_ROOT/TAO
export LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH
cd $TAO_ROOT/TAO_IDL ; time make  2>&1  | tee -a ${LOGFILE}
cd $TAO_ROOT/tao ;time make  2>&1  | tee -a ${LOGFILE}
cd $TAO_ROOT/orbsvcs ; time make  2>&1  | tee -a ${LOGFILE}
cd $TAO_ROOT/examples/Simple/echo; time make 2>&1 | tee -a ${LOGFILE}

exit 0


