/**
 * Created by chase on 11/12/2016.
 */

import com.twilio.Twilio;
import com.twilio.rest.api.v2010.account.Message;
import com.twilio.type.PhoneNuber;

public class PhoneAlert {
    //my account Sid and Token
    public static final String ACCOUNT_SID = "AC56fa4aee86c0551cf51f9b36cd4f568d";
    public static final String AUTH_TOKEN = "";

    public static void main(String[] args) {
        Twilio.init(ACCOUNT_SID, AUTH_TOKEN);

        Message message = Message.creator(new PhoneNubmer(phoneNumber),
                new PhoneNumber(phoneNumber), textMessage);

        System.out.println(message.getSid());
    }
}
