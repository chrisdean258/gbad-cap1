We are using CapitalOne's API, "Nessie," in order to simulate monetary transactions of customers, specifically money transfers between the customers. We are then running a graphics based anomally detection program, "GBAD," to find atypical transactions that may indicate fraudulent purchases unwanted by the customer. We then alert the customer via SMS message using Twilio.
 
 initCustomers.py - To be run once    
       Initializes twenty customers to api key. 
       Customers have a name, adress, and unique ID
 
initAccounts.py     
       Initializes an account for each customer.
      
simulateTransactions.py     
       This simulates each customer making one transaction each day for twenty days.
       For this simulation, the customer who receives the money in the transaction and
       the amount transferred are semi-randomly generated.
       Note: Because this is just a simulation, we planted an anomally for us to detect.
               This specific anomally involves one customer not making daily transactions,
               but rather an unexpected, one time transaction.

analyzeTransfers.py      
       Takes simulated transactions, performs some light statistical analysis and creates 
       a .g file containing its analysis
       
GBAD       
       We plug our data into GBAD, which we found as open source. GBAD performs graphics 
       based anomally detection. It compares its customer's typical transactions with data 
       from that user and other users in a similar demographic with similar spending habits.
       
sendMessage.py       
       Imports file containing detected anomally info, and uses Twilio API to alert the customer
       
deleteAccounts.py      
       Calls a delete function on the individual accounts to clean up after our program. Because 
       we are keeping this simulation small, we do not need to keep records beyond a 20 day sample
 
