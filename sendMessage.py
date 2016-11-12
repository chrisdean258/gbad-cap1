from twilio.rest import TwilioRestClient

# My Twilio Credentials
ACCOUNT_SID = "AC56fa4aee86c0551cf51f9b36cd4f568d"
AUTH_TOKEN = "0763d7872314d7e5581a8711cb7afda2"

toPhoneNumber="+13179418233"
fromPhoneNumber="+13178545402"
message="This is a test message"

client = TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN)

client.messages.create(
    to=toPhoneNumber,
    from_=fromPhoneNumber,
    body=message,
)
