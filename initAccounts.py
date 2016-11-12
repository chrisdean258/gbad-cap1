import requests
import json

class Customer:
	def __init__(self,string):
		quote = 0
		self.ID = ""
		self.firstName = ""
		self.lastName = ""
		self.streetNum = ""
		self.streetName = ""
		self.city = ""
		self.state = ""
		self.zip = ""
		while(self.ID == "" or self.firstName == "" or self.lastName == "" or self.streetNum == "" or self.streetName == "" or self.city == "" or self.state == "" or self.zip == ""):
			quote = string.find("\"",quote) + 1
			tempType = string[quote: string.find("\"", quote)]
			if (tempType == "address"):
				quote = string.find("\"",quote) + 1 	
				continue
			quote = string.find("\"",quote) + 1 	
			quote = string.find("\"",quote) + 1 	
			tempData = string[quote: string.find("\"",quote)]
			print(tempType + " " + tempData)
			quote = string.find("\"",quote) + 1 	
			self.setVal(tempType,tempData)
		self.quoteRtn = quote


	def setVal(self,dataType,data):
		if(dataType == "_id"):
			self.ID = data;
		elif(dataType == "first_name"):
			self.firstName = data
		elif(dataType == "last_name"):
			self.lastName = data
		elif(dataType == "street_number"):
			self.streetNum = data
		elif(dataType == "street_name"):
			self.streetName = data
		elif(dataType == "city"):
			self.city= data
		elif(dataType == "state"):
			self.state = data
		elif(dataType == "zip"):
			self.zip = data
	def used(self):
		return self.quoteRtn
	def getID(self):
		return self.ID
	def getFirstName(self):
		return self.firstName
	def getLastName(self):
		return self.lastName
	def getStreetNum(self):
		return self.streetNum
	def getStreetName(self):
		return self.streetName
	def getCity(self):
		return self.city
	def getState(self):
		return self.state
	def getZip(self):
		return self.zip



apiKey = '1312ae9bf58b2fd71f6632fa9c23996e'

url = 'http://api.reimaginebanking.com/customers?key={}'.format(apiKey)

customers = requests.get(url)

customerString = ""

for string in customers:
	customerString = customerString + str(string)[2:]

customerDict = {}
idList = []
while  len(customerString) > 10:
	cust = Customer(customerString)
	customerDict[cust.getID()] = cust
	idList.append(cust.getID())
	customerString = customerString[cust.used():]



for customer in customerDict:
	custObj = customerDict[customer]
	url = 'http://api.reimaginebanking.com/customers/{}/accounts?key={}'.format(customer,apiKey)
	payload = {
		"type": "Checking",
		"nickname" : custObj.getFirstName()[0] + custObj.getLastName()[0] + "Checking",
		"balance" : 10000,
		"rewards" : 0,
	}
	response = requests.post(
		url,
		data = json.dumps(payload),
		headers = {"content-type":"application/json"},
	)
	if response.status_code == 201:
		print("account created")
	else:
		print("error : no account coul be created")
