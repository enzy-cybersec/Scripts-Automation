#!/bin/bash

nmap -o result #!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: $0 <IP>"
  exit 1
fi

IP="$1"

# Run initial nmap scan, capture output and error
initial_output=$(mktemp)
initial_error=$(mktemp)

nmap -p- -T4 "$IP" -oN "$initial_output" 2> "$initial_error"
exit_code=$?

if [ $exit_code -ne 0 ]; then
  echo "[-] nmap error: $(cat "$initial_error")"
  rm "$initial_output" "$initial_error"
  exit $exit_code
fi

# Extract open ports
ports=$(grep -oP '^\d+/' "$initial_output" | cut -d'/' -f1 | sort -n | uniq | paste -sd, -)

rm "$initial_output" "$initial_error"

if [ -z "$ports" ]; then
  echo "[-] No open ports found on $IP."
  exit 1
fi
echo " "
echo "[+] nmap -p- -T4 $IP success"
echo "[*] Running detailed scan on ports: $ports"
echo " "
# Run detailed nmap scan on found ports and show output only
nmap -p "$ports" -sV -sC "$IP"

echo " "
echo "[*] UDP port scanning..."
echo " "
sudo nmap -sU "$IP"
