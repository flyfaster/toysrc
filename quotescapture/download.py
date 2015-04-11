import urllib2

def url_to_file(url, local_file, proxy):
	if len(proxy)>0:
		proxy_support = urllib2.ProxyHandler({"http" : proxy})
		opener = urllib2.build_opener(proxy_support)
		urllib2.install_opener(opener)
	print(url)
	f = urllib2.urlopen(url)
	output_file = open(local_file,'wb', 1024)
	while 1:
		data = f.read()
		if len(data) > 0:
			output_file.write(data)
		else:
			break
	output_file.close()

symbol_codes = {}
symbol_codes.setdefault("ADBE", "4112")
symbol_codes["MTW"] = "346512"
symbol_codes["ADSK"] = "30085"
symbol_codes["TEX"] = "554272"
symbol_codes["QCOM"] = "656142"
symbol_codes["NOK"] = "657729"
symbol_codes["NVDA"] = "662925"
symbol_codes["DNA"] = "663363"
symbol_codes["VRGY"] = "705307"
symbol_codes["GOOG"] = "694653"
symbol_codes["LEAP"] = "662767"
symbol_codes["PENN"] = "657712"

proxy_server = "webproxy-sn.verigy.net:80"
proxy_server = ""
for  k,v  in symbol_codes.items() :
	print(k, '=', v)
	stock_symbol = k
	stock_code = v
	http_link = str("").join(["http://finance.google.com/finance/historical?cid=",stock_code, "&startdate=Mar+27%2C+2000&enddate=Mar+25%2C+2008&output=csv"])
	save_file = str("").join(['e:/svn/stockclient/', stock_symbol, '_', stock_code, '.csv'])
	
	try:
		url_to_file(http_link, save_file, proxy_server)
		print ('download to ', save_file, ' finished')
	except IOError as e:
		print("I/O error(%s): %s" % (e.errno, e.strerror))
	except:
		print("unknown error, download ", save_file, " failed")
print("All done")