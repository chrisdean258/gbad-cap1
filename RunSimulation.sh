python3 deleteAccounts.py
python3 initAccounts.py
python3 simulateTransactions.py
python3 analyzeTransfers.py
echo "Performing Graphical Analysis"
./gbad-tool-kit_3.3/gbad-mdl_3.3/bin/gbad -mdl .35 gbadinput.g > fraudFile.txt
python sendMessage2.py
