#!/usr/bin/python3

import time
from subprocess import Popen, PIPE
import hashlib

def get_hash_from_output(raw_msg):
	msg = raw_msg.decode()
	response_beg = msg[50:].find("s:")
	response = msg[12+response_beg:-3]
	return response

def main():
	while True:
		requests = input("Please provide integers for hashing: ")
		requested_ints = [int(r) for r in requests.split()]

		for integer in requested_ints:
			p = Popen(['./client', 'send', '-n', '1', '-i', str(integer)], stdin=PIPE, stdout=PIPE, stderr=PIPE)
			output, err = p.communicate()
			if err != b"":
				print(err)
				return False

		for integer in requested_ints:
			p = Popen(['./client', 'get', '-n', '2', '-i', 'eq', str(integer), '-s', 'any'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
			output, err = p.communicate()
			if err != b"":
				print(err)
				break
			else:
				h_out = get_hash_from_output(output)
				print("Calculated hash for",integer,"is",h_out)


if __name__ == "__main__":
	main()
