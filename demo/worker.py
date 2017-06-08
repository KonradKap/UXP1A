#!/usr/bin/python3
import time
from subprocess import Popen, PIPE
import hashlib

import multiprocessing as mp

def calc_hash(msg):
	time.sleep(3)
	calc_out =  hashlib.sha224(str(msg).encode()).hexdigest()
	print("Calculated hash for",msg,":", calc_out)
	return calc_out

def get_int_from_output(raw_msg):
	return raw_msg.decode()[62:-23]

def process_request(msg):
	req_int = get_int_from_output(msg)
	h_out = calc_hash(req_int)

	p = Popen(['./client', 'send', '-n', '2', '-i', str(req_int),'-s', str(h_out)], stdin=PIPE, stdout=PIPE, stderr=PIPE)
	output, err = p.communicate()
	if err != b"":
		print(err)


def main():
	mp.set_start_method('spawn')
	while True:
		p = Popen(['./client', 'get', '-n', '1', '-i', 'any'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
		output, err = p.communicate()
		if err != b"":
			print(err)

		p = mp.Process(target=process_request, args=(output,))
		p.start()


if __name__ == "__main__":
	main()
