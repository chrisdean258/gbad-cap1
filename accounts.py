import requests
import json

class Account:
	def__init__(account, string):
		quote = 0
		account.message = ""
		account.ID = ""
		account.type = ""
		account.nickname = ""
		account.rewards = ""
		account.balance = ""
		account.account_number = ""
		account.customer_id = ""
		while(account.message == "" or account._id == "" or account.type == "" or account.nickname == "" or account.rewards == "" or account.balance = "" or account.account_number == "" or account.customer_id == ""):
			quote = string.find("\"",quote) + 1
			tempType = string[quote: string.find("\"", quote)]
			quote = string.find("\"",quote) + 1
			quote = string.find("\"",quote) + 1
			tempData = string[quote: string.find("\"",quote)]
			print(tempType + " " + tempData)
			quote = string.find("\"", quote) + 1
			account.setVal(tempType, tempData)
		account.quoteRtn = quote


	def setVal(account, dataType, data):
		if(dataType == "message"):
			account.message = data
		elif(dataType == "_id"):	
			account.ID = data
		elif(dataType == "nickname"):
			account.nickname = data
		elif(dataType == "rewards"):
			account.rewards = data
		elif(dataType == "balance"):
			account.balance = data
		elif(dataType == "account_number"):
			account.account_number = data
		elif(dataType == "customer_id"):
			account.customer_id
	
	def used(self):
		return account.quoteRtn
	
	def getMessage(account):
		return account.message

	def getID(account):
		return account.ID

	def getNickname(account):
		return account.nickname

	def getRewards(account):
		return account.rewards

	def getBalance(account):
		return account.balance

	def getAccount_number(account):
		return account.account_number

	def getCustomer_id(account):
		return account.customer


apiKey = '1312ae9bf58b2fd71f6632fa9c23996e'

url = 'http://api.reimaginebanking.com/customers?key={}'.format(apiKey)

accounts = requests.get(url)

accountString = ""

for string in accounts:
	accountString = accountString + str(string)[2:]

accountDict = {}
idList = []
while len(accountString) > 10:
	acc = Account(accountString)
	accountDict[acc.getID()] = acc
	idList.append(acc.getID())
	accountString = accountString[acc.used():]

for id in idList:
	tempID = id
	print(tempID)
	print(accountDict[id].getMessage())
	print(accountDict[id].getID())
	print(accountDict[id].getNickname())
	print(accountDict[id].getRewards())
	print(accountDict[id].getBalance())
	print(accountDict[id].getAccount_number())
	print(accountDict[id].getCustomer_id())
	
