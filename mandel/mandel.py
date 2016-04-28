# calculate mandelbrot 
import cmath
import sys

chars = [' ', '.', '|', '_']
chars = [' ', '.', ':', ';', '+', '=', 'o', 'O', '8', 'M', '#', '_']

xmin, ymin, xmax, ymax = -2.0, -0.0, 0.6, 1.2
xmin, ymin, xmax, ymax = -0.7, 0.6, -0.5, 0.7
maxit = 1000

if len(sys.argv) > 1:
	maxit = int(sys.argv[1])

cols, rows = 260, 80

def frange(start, stop, step):
	i = start
	if step < 0:
		while(i> stop):
			yield(i)
			i += step
	else:
		while(i < stop):
			yield(i)
			i += step

def mandel(x,y):
	cc = complex(x,y)
	c = cc
	for i in range(0, maxit):	
		cc = cc**2.0+c
		if (abs(cc) > 4.0):
			return i
	return i

for y in frange(ymax, ymin, -(ymax-ymin)/rows):
	for x in frange(xmin, xmax, (xmax-xmin)/cols):
		i = mandel(x,y)
		print(chars[int(i/maxit * len(chars))], end='')
	print()
