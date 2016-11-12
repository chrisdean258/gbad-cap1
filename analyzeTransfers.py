import requests
import json

class transaction:
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
		while(self.message == "" or self.id == ""  or self.Type == "" or self.transactionDate == "" or self.status == "" or self.medium == "" or self.payerID == "" or self.payeeID == ""):
			quote = string.find("\"",quote)+1
			tempType = string[quote:string.find("\"",quote)]
			if tempType == "objectCreated":
				quote = string.find("\"",quote)+1
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
