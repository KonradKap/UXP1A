#!/usr/bin/python3
import time
from subprocess import Popen, PIPE
import hashlib

def calc_hash(msg):
	time.sleep(5)
	calc_out =  hashlib.sha224(str(msg).encode()).hexdigest()
	print("Calculated hash for",msg,":", calc_out)
	return calc_out

def get_int_from_output(raw_msg):
	return raw_msg.decode()[12:-3]

def main():
	while True:
		p = Popen(['./client', 'get', '-n', '1', '-i', 'any'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
		output, err = p.communicate()
		if err != b"":
			print(err)
			break

		req_int = get_int_from_output(output)
		h_out = calc_hash(req_int)

		p = Popen(['./client', 'send', '-n', '2', '-i', str(req_int),'-s', str(h_out)], stdin=PIPE, stdout=PIPE, stderr=PIPE)
		output, err = p.communicate()
		if err != b"":
			print(err)
			break


if __name__ == "__main__":
	main()
