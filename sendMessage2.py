
from twilio.rest import TwilioRestClient

class nameIDPhone:
	def __init__(self,string):
		self.phone = "+13179418233" #chris's phone number for default
		self.name = string[:string.find(" ",string.find(" ")+1)]
		self.id = string[string.find(" ",string.find(" ")+1)+1]
		#self.num = int(string[6:string.find(" ", 6)])
		
	def getID(self):
		return self.id
	def getName(self):
		return self.name
	def getPhone(self):
		return self.phone
#	def getNum(self):
#		return self.num

# My Twilio Credentials
ACCOUNT_SID = "AC56fa4aee86c0551cf51f9b36cd4f568d"
AUTH_TOKEN = "0763d7872314d7e5581a8711cb7afda2"

# open a file named fraudFile.txt in read mode
f = open('fraudFile.txt', 'r')

anomIDs = []

anom = False
for line in f:
	if line.find("<--") != -1:
		print(line)
		quote = line.find("\"")+1
		anomIDs.append(line[quote: line.find("\"",quote)])
idDict = {}
idList = []

f= open("nameID.txt","r")
for line in f:
	temp = nameIDPhone(line)
	idDict[temp.getID()] = temp
	idList.append(temp)

# auto phone number from Twilio to send messages
fromPhoneNumber = "+13178545402"

# access the API client
client = TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN)

for i in range(len(anomIDs)):
	msg = "Attention " + str(idList[i].getName()) +"! Possibly Fraudulent activity has been detected on you account"
	client.messages.create(
		to = idList[i].getPhone(),
		from_ = fromPhoneNumber,
		body = msg,
		)
	#print("Message Sent")

		
