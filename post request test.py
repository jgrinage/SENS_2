import requests, sys
url = 'http://ec2-54-152-187-53.compute-1.amazonaws.com/uploadfile'
filename = sys.argv[1]
files = {'file': open('{}'.format(filename))}
r = requests.post(url, files=files)



