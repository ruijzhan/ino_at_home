#!/usr/bin/env python
import urllib2,time,json,subprocess,os

arduino_url = 'http://192.168.0.113'

def read_arduino(url):
    ret_string = urllib2.urlopen(url).read()
    return ret_string

def get_arduino_data(ret_string):
    lis = ret_string.split(';')
    data = {}
    for item in lis:
        pair = item.split('=')
        data[pair[0]] = int(pair[1])
    return data


if __name__ == '__main__':
    count = 0
    while True:
        try:
            data = get_arduino_data(read_arduino(arduino_url))
            lis = []
            for key in data.keys():
                lis.append({"id":key,"current_value":data[key]})

            if count%1 == 0:
                json_data = json.dumps({"version":"1.0.0", "datastreams":lis})
                with open("temp.tmp", "w") as f:
                    f.write(json_data)
                    f.close()

                subprocess.call(['curl --request PUT --data-binary @temp.tmp --header "X-ApiKey: xQ7kBxtgWZUtR4Az9cRaDa394p21j7ZxedGW9UoaGSBQh2Up" http://api.xively.com/v2/feeds/2125286213'], shell=True)
                os.remove("temp.tmp")

            time.sleep(1)
            count+=1
        except:
            print "Exception raised..."
