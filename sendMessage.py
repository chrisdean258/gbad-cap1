from twilio.rest import TwilioRestClient

# My Twilio Credentials
ACCOUNT_SID = "AC56fa4aee86c0551cf51f9b36cd4f568d"
AUTH_TOKEN = "0763d7872314d7e5581a8711cb7afda2"

# open a file named fraudFile.txt in read mode
f = open('fraudFile.txt', 'r')
# read the file into a string info
info = f.read()

# parse the phone number from the info string
toPhoneNumber = info[0:13]
# parse the message from the info string
message = info[13:]

# auto phone number from Twilio to send messages
fromPhoneNumber = "+13178545402"

# access the API client
client = TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN)

# create and send a client message with the two phone nubmers and the message
client.messages.create(
    to = str(toPhoneNumber),
    from_ = fromPhoneNumber,
    body = message,
)
