import numpy
import math
import bob.ip.gabor
import bob.sp

# create test image
image = numpy.zeros((128,128), numpy.float)
image[32:96,32:96] = 255

# compute the Gabor wavelet transform
gwt = bob.ip.gabor.Transform()
trafo_image = gwt(image)

# create image representation of all Gabor wavelets in frequency domain
wavelets_image = numpy.zeros((128,128), numpy.float)

# compute the sum over all wavelets
for wavelet in gwt.wavelets:
  wavelets_image += wavelet.wavelet

# align the wavelets so that the center is in the image center
aligned_wavelets_image = numpy.roll(numpy.roll(wavelets_image, 64, 0), 64, 1)

# create figure
import matplotlib
matplotlib.rc('text', usetex=True)
# plot wavelets image
from matplotlib import pyplot
pyplot.figure(figsize=(20,10))
pyplot.subplot(121)
pyplot.imshow(aligned_wavelets_image, cmap='gray')
pyplot.title("Family of Gabor wavelets in frequency domain")
pyplot.xticks((0, 32, 64, 96, 128), ("$-\pi$", "$-\pi/2$", "$0$", "$\pi/2$", "$\pi$"))
pyplot.yticks((0, 32, 64, 96, 128), ("$-\pi$", "$-\pi/2$", "$0$", "$\pi/2$", "$\pi$"))
pyplot.gca().invert_yaxis()

# plot the results of the transform for some wavelets
for scale in (0,2,4):
  for direction in (0,2,4):
    pyplot.subplot(3,6,4+scale*3+direction/2)
    pyplot.imshow(numpy.real(trafo_image[scale*gwt.number_of_directions+direction]), cmap='gray')
    pyplot.title("Scale %d, direction %d" % (scale, direction))
    pyplot.gca().invert_yaxis()

pyplot.show()

