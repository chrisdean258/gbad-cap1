import requests
import json

apiKey = "fcdf2af0ab8427bcac93139bb0775400"

url = 'http://api.reimaginebanking.com/customers?key={}'.format(apiKey)
#1
customerPayload = {
	"first_name": "John",
		"last_name": "Doe",
		"address": {
			"street_number": "520",
			"street_name": "Scenic Drive",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#2
customerPayload = {
	"first_name": "Kathryn",
		"last_name": "Bishop",
		"address": {
			"street_number": "4171",
			"street_name": "Morris Street",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#3
customerPayload = {
	"first_name": "Frank",
		"last_name": "Mason",
		"address": {
			"street_number": "1644",
			"street_name": "Ryder Avenue",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#4
customerPayload = {
	"first_name": "Elizabeth",
		"last_name": "Whitley",
		"address": {
			"street_number": "578",
			"street_name": "Jones Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#5
customerPayload = {
	"first_name": "Julia",
		"last_name": "Dahl",
		"address": {
			"street_number": "2270",
			"street_name": "Oakmound Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#6
customerPayload = {
	"first_name": "Julie",
		"last_name": "Barnes",
		"address": {
			"street_number": "2959",
			"street_name": "Grant View Drive",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#7
customerPayload = {
	"first_name": "Richard",
		"last_name": "Dunning",
		"address": {
			"street_number": "2395",
			"street_name": "Midway Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#8
customerPayload = {
	"first_name": "William",
		"last_name": "Singleton",
		"address": {
			"street_number": "861",
			"street_name": "Poling Farm Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#9
customerPayload = {
	"first_name": "Kelly",
		"last_name": "Price",
		"address": {
			"street_number": "2177",
			"street_name": "Oakridge Lane",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#10
customerPayload = {
	"first_name": "Louis",
		"last_name": "Schultz",
		"address": {
			"street_number": "3711",
			"street_name": "Prospect Street",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#11
customerPayload = {
	"first_name": "Sadie",
		"last_name": "Williams",
		"address": {
			"street_number": "3087",
			"street_name": "Perine Street",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#12
customerPayload = {
	"first_name": "Anthony",
		"last_name": "Douglas",
		"address": {
			"street_number": "4232",
			"street_name": "Maud Street",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#13
customerPayload = {
	"first_name": "Earl",
		"last_name": "Rhodes",
		"address": {
			"street_number": "3983",
			"street_name": "Horizon Circle",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#14
customerPayload = {
	"first_name": "John",
		"last_name": "Cole",
		"address": {
			"street_number": "4518",
			"street_name": "Hawkeye Lane",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#15
customerPayload = {
	"first_name": "Estelle",
		"last_name": "Babbit",
		"address": {
			"street_number": "1881",
			"street_name": "School House Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#16
customerPayload = {
	"first_name": "Jennifer",
		"last_name": "Galindo",
		"address": {
			"street_number": "296",
			"street_name": "PrettyView Lane",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#17
customerPayload = {
	"first_name": "Thomas",
		"last_name": "Singer",
		"address": {
			"street_number": "479",
			"street_name": "Red Hawk Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#18
customerPayload = {
	"first_name": "Todd",
		"last_name": "Embry",
		"address": {
			"street_number": "1474",
			"street_name": "Gladwell Street",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#19
customerPayload = {
	"first_name": "Lenard",
		"last_name": "Knight",
		"address": {
			"street_number": "791",
			"street_name": "Sycamore Road",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')

#20
customerPayload = {
	"first_name": "Nathan",
		"last_name": "Kuhn",
		"address": {
			"street_number": "2173",
			"street_name": "Spruce Street",
			"city": "Nashville",
			"state": "TN",
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
else:
	print('customer not created')
