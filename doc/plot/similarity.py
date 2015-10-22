import numpy
import bob.io.base
import bob.io.base.test_utils
import bob.ip.gabor

# load test image
image = bob.io.base.load(bob.io.base.test_utils.datafile("testimage.hdf5", 'bob.ip.gabor'))
# perform Gabor wavelet transform on image
gwt = bob.ip.gabor.Transform()
trafo_image = gwt(image)
# extract Gabor jet at right eye location
pos = (177, 131)
eye_jet = bob.ip.gabor.Jet(trafo_image, pos)

# compute similarity field over the whole image
cos_sim = bob.ip.gabor.Similarity("ScalarProduct")
disp_sim = bob.ip.gabor.Similarity("Disparity", gwt)
cos_image = numpy.zeros(((image.shape[0]+1)//4, (image.shape[1]+1)//4))
disp_image = numpy.zeros(((image.shape[0]+1)//4, (image.shape[1]+1)//4))
# .. re-use the same Gabor jet object to avoid memory allocation
image_jet = bob.ip.gabor.Jet()
for y in range(2, image.shape[0], 4):
  for x in range(2, image.shape[1], 4):
    image_jet.extract(trafo_image, (y,x))
    cos_image[y//4,x//4] = cos_sim(image_jet, eye_jet)
    disp_image[y//4,x//4] = disp_sim(image_jet, eye_jet)

# plot the image and the similarity map side-by-side
from matplotlib import pyplot
pyplot.figure(figsize=(20,7))
pyplot.subplot(131)
pyplot.imshow(image, cmap='gray')
pyplot.plot(pos[1], pos[0], "gx", markersize=20, mew=4)
pyplot.axis([0, image.shape[1], image.shape[0], 0])
#pyplot.axis("scaled")
pyplot.title("Original image")

pyplot.subplot(132)
pyplot.imshow(cos_image, cmap='jet')
pyplot.title("Similarities (cosine)")

pyplot.subplot(133)
pyplot.imshow(disp_image, cmap='jet')
pyplot.title("Similarities (disparity)")

pyplot.show()
