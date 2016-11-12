from twilio.rest import TwilioRestClient

# My Twilio Credentials
ACCOUNT_SID = "AC56fa4aee86c0551cf51f9b36cd4f568d"
AUTH_TOKEN = "0763d7872314d7e5581a8711cb7afda2"

f = open('fraudFile.txt', 'r')
info = f.read()

toPhoneNumber = info[0:13]
message = info[13:]

fromPhoneNumber = "+13178545402"

print(info)
print(toPhoneNumber)
print(message)

client = TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN)

client.messages.create(
    to = str(toPhoneNumber),
    from_ = fromPhoneNumber,
    body = message,
)
