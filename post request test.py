import requests
import sys
url = 'http://ec2-54-152-187-53.compute-1.amazonaws.com/uploadfile'
files = {'file': open(sys.argv)}
r = requests.post(url, files=files)


