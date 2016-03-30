import requests
url = 'http://ec2-54-152-187-53.compute-1.amazonaws.com/uploadfile'
files = {'file' : 'Bike_1.csv'}
r = requests.post(url, files=files)

