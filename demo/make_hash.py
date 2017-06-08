#!/usr/bin/python3

import time
from subprocess import Popen, PIPE
import hashlib

def get_hash_from_output(raw_msg):
	msg = raw_msg.decode()
	response_beg = msg[50:].find("s:")
	response = msg[52+response_beg:-23]
	return response

def main():
	while True:
		requests = input("Please provide positive integers for hashing: ")
		try:
			requested_ints = [int(r) for r in requests.split()]
			negatives = sum([r < 0 for r in requested_ints])
			if negatives > 0:
				raise Exception("Negative integer was provided")
		except Exception as e:
			print("Could not convert provided input to list of integers, reason was:",e)
			continue

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
