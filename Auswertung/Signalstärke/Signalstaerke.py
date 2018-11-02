#!/usr/bin/env python

import sys
import pprint
import requests
import xmltodict
import time

class HuaweiE3372(object):
  BASE_URL = 'http://{host}'
  COOKIE_URL = '/html/index.html'
  XML_APIS = [
    '/api/device/information',
    '/api/device/signal',
    '/api/monitoring/status',
    '/api/monitoring/traffic-statistics',
    '/api/dialup/connection',
    '/api/global/module-switch',
    '/api/net/current-plmn',
    '/api/net/net-mode',
  ]
  session = None

  def __init__(self,host='192.168.8.1'):
    self.host = host
    self.base_url = self.BASE_URL.format(host=host)
    self.session = requests.Session()
    # get a session cookie by requesting the COOKIE_URL
    r = self.session.get(self.base_url + self.COOKIE_URL)

  def get(self,path):
    return xmltodict.parse(self.session.get(self.base_url + path).text).get('response',None)

def main():
  e3372 = HuaweiE3372()
  print "<data>"
  n = 0
  while n < 30:
    print "<set time=" , time.time(),">"

    for path in e3372.XML_APIS:
      for key,value in e3372.get(path).items():

      #print(key,value)
        if(key==u'FullName'):
          print "<fullname>", value, "</fullname>"
        if(key==u'workmode'):
          print "<workmode>", value, "</workmode>"
        if(key==u'rsrq'):
          print "<rsrq>", value, "</rsrq>"
        if(key==u'rssi'):
          print "<rssi>", value, "</rssi>"
        if(key==u'sinr'):
          print "<sinr>", value, "</sinr>"
        if(key==u'rsrp'):
          print "<rsrp>", value, "</rsrp>"
    print "</set>"
    n =+ 1
    time.sleep(1)
  print "</data>"
if __name__ == "__main__":
  main()

