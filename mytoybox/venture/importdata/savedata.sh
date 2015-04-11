MY_PATH="`dirname \"$0\"`"              # relative
MY_PATH="`( cd \"$MY_PATH\" && pwd )`"  # absolutized and normalized
if [ -z "$MY_PATH" ] ; then
  exit 1  # fail
fi

declare -a arr=("GOOG" "C" "MSFT" "QQQ")

for i in "${arr[@]}"
do
   echo "$i"
   # or do whatever with individual element of the array
   phantomjs --load-plugins=true ${MY_PATH}/load-google-finance.js "http://www.google.com/finance?chdnp=1&chdd=1&chds=1&chdv=1&chvs=maximized&chdeh=0&chfdeh=0&chdet=1423200847674&chddm=1172.9999999999998&chls=IntervalBasedLine&q=NASDAQ%3A${i}&ntsp=0&fct=big&ei=TlLUVJnyAdDlqAG7r4H4Dg" chart_anchor > $i-$(date +%Y%m%d).txt
   if [ $? -ne 0 ]; then
    echo "failed to load $i"
  fi
done

