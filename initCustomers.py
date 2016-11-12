import requests
import json

apiKey = "1312ae9bf58b2fd71f6632fa9c23996e"


url = 'http://api.reimaginebanking.com/customers?key={}'.format(apiKey)

customerPayload = {
	"first_name": "john",
		"last_name": "doe",
		"address": {
			"street_number": "520",
			"street_name": "scenic drive",
			"city": "nashville",
			"state": "tn",
			"zip": "38962"
		}
}



response = requests.post(
	url,
	data = json.dumps(customerPayload),
	headers={'content-type':'application/json'},
	)
if response.status_code == 201:
	print('customer created')
