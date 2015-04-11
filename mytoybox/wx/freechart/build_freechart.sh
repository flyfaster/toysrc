tar xf ~/Downloads/freechart-1.6.tar.gz

if (($? > 0)); then    
    echo "Please download freechart-1.6.tar.gz from http://sourceforge.net/projects/wxcode/files/Components/wxFreeChart/"
    exit $?
fi

if [ -L $0 ] ; then
    SCRIPTDIR=$(dirname $(readlink -f $0)) ;
else
    SCRIPTDIR=$(dirname $0) ;
fi ;

cd freechart
if (($? > 0)); then    
    echo "Please check untar result"
    exit $?
fi

cp -pv ${SCRIPTDIR}/configure .
if (($? > 0)); then    
    echo "Please check copy error"
    exit $?
fi

cp -pv ${SCRIPTDIR}/areadraw.h include/wx
if (($? > 0)); then    
    echo "Please check copy error"
    exit $?
fi

./configure 2>&1 | tee -a build-$(date +%Y%m%d%H%M).txt

make 2>&1 | tee -a build-$(date +%Y%m%d%H%M).txt

echo "run the following program to check build result"
echo "LD_LIBRARY_PATH=lib:\$LD_LIBRARY_PATH sample/wxFreeChartDemo_dll"

