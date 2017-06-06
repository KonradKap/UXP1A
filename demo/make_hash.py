#!/usr/bin/python3

import time
from subprocess import Popen, PIPE
import hashlib

def get_hash_from_output(raw_msg):
	msg = raw_msg.decode()
	response_beg = msg[10:].find("s:")
	response = msg[12+response_beg:-3]
	return response

def main():
	while True:
		request = raw_input("Please provide integer for hashing: ")
		requested_int = int(request)

		p = Popen(['./client', 'send', '-n', '1', '-i', str(requested_int)], stdin=PIPE, stdout=PIPE, stderr=PIPE)
		output, err = p.communicate()
		if err != b"":
			print(err)
			break

		p = Popen(['./client', 'get', '-n', '2', '-i', 'eq', str(requested_int), '-s', 'any'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
		output, err = p.communicate()
		if err != b"":
			print(err)
			break
		else:
			h_out = get_hash_from_output(output)
			print("Calculated hash is",h_out)


if __name__ == "__main__":
	main()
