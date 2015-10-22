import bob.ip.gabor
import bob.io.base
import bob.io.base.test_utils

# load test image
image = bob.io.base.load(bob.io.base.test_utils.datafile("testimage.hdf5", 'bob.ip.gabor'))
# perform Gabor wavelet transform on image
gwt = bob.ip.gabor.Transform(number_of_scales=9)
trafo_image = gwt(image)
# extract reference Gabor jet at nose tip location
pos = (228, 173)
reference_jet = bob.ip.gabor.Jet(trafo_image, pos)

r = 40
# extract test region from image (for displaying purposes
region = image[pos[0]-r:pos[0]+r+1, pos[1]-r:pos[1]+r+1]

# display test region
from matplotlib import pyplot
pyplot.figure(figsize=(7,7))
pyplot.imshow(region, cmap='gray')
pyplot.axis([0, region.shape[1]-1, region.shape[0]-1, 0])
pyplot.xticks(range(0,81,20), range(-40,41,20))
pyplot.yticks(range(0,81,20), range(-40,41,20))

# create a disparity-based similarity function
similarity = bob.ip.gabor.Similarity("Disparity", gwt)
cmap=pyplot.get_cmap("jet")

# compute disparities of all gabor jets around the current location
for y in range(-30, 31, 6):
  for x in range(-30, 31, 6):
    if x or y:
      # extract Gabor jet at location
      jet = bob.ip.gabor.Jet(trafo_image, (pos[0] + y, pos[1] + x))
      # compute the similarity between the two jets
      sim = similarity(reference_jet, jet)
      # get the disparity of the two jets
      disp = similarity.last_disparity

      # normalize similarity from range [-1, 1] to range [0,1] to obtain a proper color coding
      color = cmap((sim + 1.) / 2.)
      # plot the disparity as an arrow from the current location, with the given similarity as a color code
      pyplot.arrow(x+r, y+r, disp[1], disp[0], color = color, head_width=2)

pyplot.show()
