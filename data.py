# This is written for PYTHON 3
# Don't forget to install requests package

import requests
import json

customerId = '1312ae9bf58b2fd71f6632fa9c23996e'
apiKey = '1312ae9bf58b2fd71f6632fa9c23996e'

url = 'http://api.reimaginebanking.com/customers/{}/accounts?key={}'.format(customerId,apiKey)
print(url)


response = requests.get( 
		"http://api.reimaginebanking.com/accounts?key=1312ae9bf58b2fd71f6632fa9c23996e", 
		headers={'content-type':'application/json'},
		)

print(response)
payload = {
		"type": "Credit Card",
		"nickname": "a",
		"rewards": 0,
		"balance": 0,
		"account_number": "123"
	}

# Create a Savings Account
response = requests.post( 
		url, 
		data=json.dumps(payload),
		headers={'content-type':'application/json'},
		)

if response.status_code == 201:
	print('account created')
if response.status_code == 404:
	print('account not created')
	print(response)
