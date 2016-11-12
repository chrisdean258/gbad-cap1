import requests
import json

class Account:
	def __init__(self, string):
		quote = 0
		self.message = ""
		self.ID = ""
		self.type = ""
		self.nickname = ""
		self.rewards = ""
		self.balance = ""
		self.accountNumber = ""
		self.customerID = ""
		while( self.ID == "" or self.type == "" or self.nickname == "" or self.rewards == "" or self.balance == "" or self.customerID == ""):
			quote = string.find("\"",quote) + 1
			tempType = string[quote: string.find("\"", quote)]
			if tempType == "objectCreated":
				quote = string.find("\"",quote) + 1
				continue
			elif tempType == "balance":
				quote = string.find("\"",quote) + 1
				tempData = string[quote+1:string.find(",",quote+1)]
				#print(tempType + " " + tempData)
				self.setVal(tempType,tempData)
				continue
			elif tempType == "rewards":
				quote = string.find("\"",quote) + 1
				tempData = string[quote+1:string.find(",",quote+1)]	
				self.setVal(tempType,tempData)
				#print(tempType + " " + tempData)
				continue
			quote = string.find("\"",quote) + 1
			quote = string.find("\"",quote) + 1
			tempData = string[quote: string.find("\"",quote)]
			#print(tempType + " " + tempData)
			quote = string.find("\"", quote) + 1
			self.setVal(tempType, tempData)
		self.quoteRtn = quote


	def setVal(self, dataType, data):
		if(dataType == "message"):
			self.message = data
		elif(dataType == "_id"):	
			self.ID = data
		elif(dataType == "nickname"):
			self.nickname = data
		elif(dataType == "rewards"):
			self.rewards = data
		elif(dataType == "balance"):
			self.balance = data
		elif(dataType == "account_number"):
			self.accountNumber = data
		elif(dataType == "customer_id"):
			self.customerID = data
		elif(dataType == "type"):
			self.type = data
	
	def used(self):
		return self.quoteRtn
	
	def getMessage(self):
		return self.message

	def getID(self):
		return self.ID

	def getNickname(self):
		return self.nickname

	def getRewards(self):
		return self.rewards

	def getBalance(self):
		return self.balance

	def getAccountNumber(self):
		return self.accountNumber

	def getCustomerID(self):
		return self.customerID


apiKey = '1312ae9bf58b2fd71f6632fa9c23996e'

url = 'http://api.reimaginebanking.com/accounts?key={}'.format(apiKey)

accounts = requests.get(url)

accountStringtemp = ""

for string in accounts:
	accountStringtemp = accountStringtemp + str(string)[2:]
	accountString = accountStringtemp.replace("'", "")

accountDict = {}
idList = []
while len(accountString) > 10:
	acc = Account(accountString)
	accountDict[acc.getID()] = acc
	idList.append(acc.getID())
	accountString = accountString[acc.used():]

for id in idList:
	url = 'http://api.reimaginebanking.com/accounts/{}?key={}'.format(id,apiKey)
	retval = requests.delete(url)
	print(retval)

