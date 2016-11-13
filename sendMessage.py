from twilio.rest import TwilioRestClient

# My Twilio Credentials
ACCOUNT_SID = "ACe815d90767f07086bfbe45235d2b2db3"
AUTH_TOKEN = "708f97c7f375f76e74802c4423664f5d"

client = TwilioRestClient(ACCOUNT_SID, AUTH_TOKEN)
# open a file named fraudFile.txt in read mode
ff = open('fraudFile.txt', 'r')

names = open('nameID.txt', 'r')
# read the file into a string info
# info = f.read()
fromPhoneNumber = "+15005550006"
toPhoneNumber = "+16155120921"
message = "HELLO"
name = ""

for line in ff:
    temp = ff.readline();
    if "v" in temp[0:6]:
        if "<--" not in temp:
            id_ = temp[temp.index("\"") + 1:]
            id_ = id_[0:24]
            print(id_)
            for line in names:
                name = names.readline()
                print(name)
                if name[0:25] == id_:
                    message = name[25:name.index(" ")]
                    name = name[name.index(" ") + 1]
                    name = name[name.index(" ") + 1]
                    message = message + name[0:name.index(" ")]
                    print(message)
                    name = name[name.index(" ")]
#                    toPhoneNumber = "+13179418233"
            

# access the API client

# create and send a client message with the two phone nubmers and the message
    client.messages.create(
        to = toPhoneNumber,
        from_ = fromPhoneNumber,
        body = message,
    )
