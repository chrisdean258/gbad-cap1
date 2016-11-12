import requests
import json

class Transfer:
	def __init__(self,string):
		quote = 0
		self.message = ""
		self.id = ""
		self.Type = ""
		self.transactionDate = ""
		self.status = ""
		self.medium = ""
		self.payerID = ""
		self.payeeID = ""
		while(self.id == ""  or self.Type == "" or self.transactionDate == "" or self.status == "" or self.medium == "" or self.payerID == "" or self.payeeID == ""):
			quote = string.find("\"",quote)+1
			tempType = string[quote:string.find("\"",quote)]
			if tempType == "objectCreated":
				quote = string.find("\"",quote)+1
				continue
			if tempType == "description":
				quote = string.find("\"",quote)+1
				continue
			elif tempType == "amount":	
				quote = string.find("\"",quote)+1
				tempData = string[quote+2:string.find(",",quote)]
#				quote = string.find("\"",quote)+1
				continue
			quote = string.find("\"",quote)+1
			quote = string.find("\"",quote)+1
			tempData = string[quote:string.find("\"",quote)]
			print(tempType + " " + tempData)
			quote = string.find("\"",quote)+1
			self.setVal(tempType,tempData)
		self.quoteRtn = quote

	def setVal(self,dataType,data):
		if(dataType == "_id"):
			self.id = data
		elif(dataType == "message"):
			self.message = data
		elif(dataType == "type"):
			self.type = data
		elif(dataType == "transaction_date"):
			self.transactionDate = data
		elif(dataType == "status"):
			self.status = data
		elif(dataType == "medium"):
			self.medium = data
		elif(dataType == "payer_id"):
			self.payerID = data
		elif(dataType == "payee_id"):
			self.payeeID = data	
	def used(self):
		return self.quoteRtn
	def getID(self):
		return self.id
	def getMessage(self):
		return self.message
	def getType(self):
		return self.type
	def getTransactionDate(self):
		return self.transactionDate
	def getStatus(self):
		return self.status
	def getMedium(self):
		return self.medium
	def getPayerID(self):
		return self.payerID
	def getPayeeID(self):
		return self.payeeID

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

transferID = []
for ID in idList:
	url2 = 'http://api.reimaginebanking.com/accounts/{}/transfers?type=payer&key={}'.format(ID,apiKey)
	transfers = requests.get(url2)
	transferStringtemp = ""
	for string in transfers:
		transferStringtemp = transferStringtemp + str(string)[2:]
		transferString = transferStringtemp.replace("'","")
	while len(transferString) > 10:
		tran = Transfer(transferString)
		transferID.append(tran.getID())
		print(tran.getID())

