import sys
import os

def MyFunc(some_arg):
    result = some_arg + ' from python' + sys.version + ' on ' + sys.platform
    print('sys.stdout encoding is "' + sys.stdout.encoding + '"')
    print ( ', '.join(sys.modules.keys()) )
    print ( 'sys.path='+ ', '.join(sys.path) )
    return result

def MyFunca():
    result = ' from python.'
    print('sys.stdout encoding is "' + sys.stdout.encoding + '"')
#    if sys.maxunicode < 65535:
#        print result
    return result
