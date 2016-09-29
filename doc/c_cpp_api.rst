.. vim: set fileencoding=utf-8 :
.. Manuel Guenther <manuek.guether@idiap.ch>
.. Fri Jun 13 09:53:03 CEST 2014

===========
 C/C++ API
===========

This section includes information for using the C or C++ API of ``bob::ip::gabor``.
The pure `C++ API`_ of the classes in ``bob::ip::gabor`` can be obtained through including the specific header files, which will be described below, e.g.:

.. code-block:: c++

   #include <bob.ip.gabor/Wavelet.h>

On the other hand, to leverage from the pure `C API`_, you simply need to include:

.. code-block:: c++

   #include <bob.ip.gabor/api.h>


C++ API
-------

The classes and functions in the C++ API are all contained in the ``bob::ip::gabor`` namespace.
All classes from the :ref:`bob.ip.gabor_py_api` are reflected in the C++ API as well.
Particularly:

Gabor wavelet
+++++++++++++

.. cpp:class:: bob::ip::gabor::Wavelet

   Implements the Gabor wavelet in C++, which is defined in frequency domain as:

   .. math::
      :label: wavelet

      \check\psi_{\vec k}(\vec \omega) = k^{\lambda} \left\{ e^{-\frac{\sigma^2(\vec \omega - \vec k)^2}{2\vec k^2}} - {e^{-\frac{\sigma^2(\vec \omega^2 + \vec k^2)}{2\vec k^2}}}\right\}

   .. function:: Wavelet(\
        const blitz::TinyVector<int,2>& resolution,\
        const blitz::TinyVector<double,2>& wavelet_frequency,\
        const double sigma = 2. * M_PI,\
        const double pow_of_k = 0.,\
        const bool dc_free = true,\
        const double epsilon = 1e-10\
      )

      Constructor taking the image resolution, the wavelet frequency :math:`\vec k`, the width of the enveloping Gaussian :math:`\sigma` and the :math:`\lambda` value ``power_of_k``.
      When ``dct_free`` is set to ``false``, the second part of :eq:`wavelet` will not be added.
      For efficiency reasons, the Gabor wavelet is not implemented as an image, but wavelet values that are lower than the given ``epsilon`` are discarded.

   .. function:: blitz::Array<double,2> waveletImage() const

      Computes and returns an image containing the Gabor wavelet in frequency domain.

   .. function:: transform(\
        const blitz::Array<std::complex<double>,2>& frequency_domain_image,\
        blitz::Array<std::complex<double>,2>& transformed_frequency_domain_image\
      ) const

      Performs the Gabor wavelet transform with a single Gabor wavelet on the given ``frequency_domain_image`` and writes it's result into the ``transformed_frequency_domain_image``.
      Note that both images are of complex type and considered to be in frequency domain.

Gabor wavelet family
++++++++++++++++++++

.. cpp:class:: bob::ip::gabor::Transform

   Implements a family of Gabor wavelets used to perform a Gabor wavelet transform.
   Gabor wavelets are extracted for a given number of scales and directions, which will result in several :math:`\vec k_j` vectors for the :eq:`wavelet`:

   .. math::
      :label: family

      \vec k_j = k_{\zeta} \left\{ \begin{array}{c} \cos \vartheta_{\nu} \\ \sin \vartheta_{\nu} \end{array}\right\}

   with:

   .. math::
      :label: family_member

      k_{\zeta} = k_{max} \cdot k_{fac}^{\zeta} \qquad \zeta = \{0,\dots,\zeta_{max}-1\} \qquad \vartheta_{\nu} = \frac{\nu \cdot 2\pi}{\nu_{max}}` \qquad \nu = \{0,\dots,\nu_{max}-1\}


   .. function:: Transform(\
        int number_of_scales = 5,\
        int number_of_directions = 8,\
        double sigma = 2. * M_PI,\
        double k_max = M_PI / 2.,\
        double k_fac = 1./sqrt(2.),\
        double pow_of_k = 0.,\
        bool dc_free = true,\
        double epsilon = 1e-10\
      )

      Constructor taking the ``number_of_scales`` :math:`\zeta_{max}`, the ``number_of_orientations`` :math:`\nu_{max}`, the ``k_max`` :math:`k_{max}` and the ``k_fac`` :math:`k_{fac}` as given in :eq:`family_member`.
      The parameters ``sigma``, ``pow_of_k``, ``dc_free`` and ``epsilon`` are directly passed to the :cpp:class:`Wavelet` constructor.

      .. note::
         The Gabor wavelets are not generated in this constructor since the image resolution is not known at construction time.
         The wavelets will only be generated during a call to `transform` or to `generateWavelets`.


   .. function:: void transform(const blitz::Array<T,2>& gray_image, blitz::Array<std::complex<double>,3>& trafo_image)

      Computes a Gabor wavelet transform on the given image, which can be of various types ``T``.
      If needed, this function will automatically call `generateWavelets` with the current image resolution.
      The resulting ``trafo_image`` must have the shape (`numberOfWavelets`, ``grap_image.extent(0)``, ``grap_image.extent(1)``).

   .. function:: void generateWavelets(int y_resoultion, int x_resolution)

      Generates the family of Gabor wavelets for the given image resolution.

   .. :function:: std::vector<blitz::TinyVector<double,2> >& waveletFrequencies() const

      Returns the list of frequencies :math:`\vec k_j` as defined in :eq:`family` for all wavelets stored in this family.

   .. function:: std::vector<boost::shared_ptr<bob::ip::gabor::Wavelet>>& wavelets() const

      Provides the list of :cpp:class:`Wavelet`\s which are stored in this class.

      .. note::
         This list will be empty until either of  `transform` or `generateWavelets` is called.

   .. function:: int numberOfWavelets() const

      Returns the number of wavelets of this Gabor wavelet family, i.e., :math:`\zeta_{max} \cdot \nu_{max}`.

   .. function:: void load(bob::io::base::HDF5File& file)

      Loads the configuration of this Gabor wavelet family from the given `bob::io::base::HDF5File`.

      .. note:: No wavelets are created in after loading the configuration.

   .. function:: void save(bob::io::base::HDF5File& file) const

      Saves the configuration of this Gabor wavelet family to the given `bob::io::base::HDF5File`.

Gabor jet
+++++++++

.. cpp:class:: bob::ip::gabor::Jet

   An implementation of a Gabor jet, which is a local texture descriptor by collecting all wavelet responses of a given :cpp:class:`Transform` object.
   Gabor jets store the wavelet responses in polar form, i.e., storing the absolute values `abs` and the phases `phase`.
   In this implementation, the Gabor phases are **always** extracted, and by default, the vector of absolute values of the Gabor jets is normalized to unit Euclidean length.

   .. function:: Jet(int length = 0)

      Default constructor that generates an uninitialized Gabor jet of the given length

   .. function:: Jet(const blitz::Array<std::complex<double>,3>& trafo_image, const blitz::TinyVector<int,2>& position, bool normalize = true)

      Extracts a Gabor jet at the given ``location`` from the ``trafo_image``, which usually is a result of the `Transform::transform` function.

   .. function:: Jet(const blitz::Array<std::complex<double>,1>& data, bool normalize = true)

      Creates a Gabor jet from the given vector of complex-valued data.

   .. function:: Jet(const std::vector<boost::shared_ptr<bob::ip::gabor::Jet>>& jets, bool normalize = true)

      Creates a Gabor jet by averaging the given Gabor jets, which need to be of the same length.

   .. function:: double normalize()

      Normalizes the absolute values of the Gabor jet to unit Euclidean length and return its old Euclidean length.

   .. function:: blitz::Array<double,2>& jet()

      Returns the absolute and phase values of this Gabor jet, where ``jet()(0,.)`` contains the absolute values, while ``jet()(1,.)`` comprises the phases.

   .. function:: const blitz::Array<double,1>& abs()

      Returns the absolute values of this Gabor jet, i.e., `jet` ``(0)``.

   .. function:: const blitz::Array<double,1>& phase()

      Returns the phase values of this Gabor jet, i.e., `jet` ``(1)``.

   .. function:: const blitz::Array<std::complex<double>,1> complex() const

      Returns a complex-valued representation of the Gabor jet, which is computed on the fly.

   .. function:: int length() const

      Returns the length of this Gabor jet, which is usually the number of wavelets `Transform::numberOfWavelets`, i.e., :math:`\zeta_{max} \cdot \nu_{max}`.

   .. function:: void load(bob::io::base::HDF5File& file)

      Loads the Gabor jet from the given `bob::io::base::HDF5File`.

   .. function:: void save(bob::io::base::HDF5File& file) const

      Saves the Gabor jet to the given `bob::io::base::HDF5File`.


Gabor jet similarity
++++++++++++++++++++

.. cpp:class:: bob::ip::gabor::Similarity

   Implements several Gabor jet similarity functions, which will compute the similarity of two :cpp:class:`Jet`\s.
   Currently, several types are implemented, see the documentation for the Python class :py:class:`bob.ip.gabor.Jet` for a list of implemented functions.

   .. cpp:class:: SimilarityType

      Enumeration to define the type of the similarity function to be computed.

   .. function:: Similarity(SimilarityType type, boost::shared_ptr<Transform> gwt = boost::shared_ptr<Transform>())

      Constructor to create a Gabor jet similarity function of the given :cpp:class:`SimilarityType`.
      Some types of similarity functions require the :cpp:class:`Transform` with which the :cpp:class:`Jet`\s are extracted.

   .. function:: double similarity(const Jet& jet1, const Jet& jet2) const

      Computes the similarity of the two Gabor jets using.

   .. function:: blitz::TinyVector<double,2> disparity(const Jet& jet1, const Jet& jet2) const

      Estimates the disparity vector between the given two Gabor jets.
      For some similarity functions, the `disparity` is computed and stored.

   .. function:: blitz::TinyVector<double,2> disparity() const

      Returns the disparity vector estimated in the last call to `similarity`.

      .. note::
         Not all similarity function compute the disparity.
         Hence, the returned values might be ``NaN``.

   .. function:: shift_phase(const Jet& jet, const Jet& reference, Jet& shifted) const

      Shifts the `Jet::phase` values of the ``jet`` towards the ``reference`` such that the ``disparity(shifted, reference) == (0., 0.)``.

   .. function:: void load(bob::io::base::HDF5File& file)

      Loads the configuration of this Gabor jet similarity from the given `bob::io::base::HDF5File`.

   .. function:: void save(bob::io::base::HDF5File& file) const

      Saves the configuration of this Gabor jet similarity to the given `bob::io::base::HDF5File`.

Gabor graph
+++++++++++

.. cpp:class:: bob::ip::gabor::Graph

   Extracts several Gabor jets from a given image using a fixed set of locations, which usually form a grid.

   .. function:: Graph(\
        blitz::TinyVector<int,2> righteye,\
        blitz::TinyVector<int,2> lefteye,\
        int between,\
        int along,\
        int above,\
        int below\
      )

      Generates a grid graph extractor which is aligned to the given eye positions.
      When the eye positions are not on a horizontal line, the grid will be slanted.
      In the graph, there will be ``between`` nodes placed in between the eye positions, ``along`` nodes to the left and to the right of the eyes, ``above`` nodes above the eyes and ``below`` nodes below the eyes.
      Hence, in total ``(2*along + between + 2) X (above + below + 1)`` nodes will be created.

   .. function:: Graph(blitz::TinyVector<int,2> first, blitz::TinyVector<int,2> last, blitz::TinyVector<int,2> step)

      Generates a grid graph extractor which will extract regular grid graphs.
      The first node is extracted at the given ``first`` position, the next nodes will be placed ``step`` pixels further (where horizontal and vertical steps are handled independently), and the last node is placed at **or before** the given ``last`` node.

   .. function:: Graph(const std::vector<blitz::TinyVector<int,2>>& nodes)

      Constructs a graph extractor using the given nodes.

   .. function:: void extract(const blitz::Array<std::complex<double>,3> trafo_image, std::vector<boost::shared_ptr<Jet>>& jets, bool normalize = true) const

      Extracts Gabor jets from the given ``trafo_image`` (which is usually the result of a call to `Transform::transform`.
      The extracted Gabor jets will be placed into the given ``jets`` vector, which might be empty or contain Gabor jets, which will be updated.

   .. function:: nodes(const std::vector<blitz::TinyVector<int,2>>& nodes)

      Replaces the nodes of this graph with the given ones.

   .. function:: const std::vector<blitz::TinyVector<int,2>>& nodes() const

      Returns the node positions of this graph.

   .. function:: void load(bob::io::base::HDF5File& file)

      Loads the configuration of this graph extractor from the given `bob::io::base::HDF5File`.

   .. function:: void save(bob::io::base::HDF5File& file) const

      Saves the configuration of this graph extractor to the given `bob::io::base::HDF5File`.


C API
-----

The C-API can be used in the Python bindings, when you need to reference a C++ class of the ``bob::ip::gabor`` namespace.
When you use the C++ API in a derived package Python bindings, please make sure that you import this package, as well as the dependencies :ref:`bob.blitz <bob.blitz>`, :ref:`bob.io.base <bob.io.base>` and :ref:`bob.sp <bob.sp>`, by adding the following lines to your module definition:

.. code-block:: c

   #include <bob.blitz/capi.h>
   #include <bob.io.base/api.h>
   #include <bob.sp/api.h>
   #include <bob.ip.gabor/api.h>

   PyMODINIT_FUNC initclient(void) {

     ...

     /* imports dependencies */
     if (import_bob_blitz() < 0) {
       PyErr_Print();
       PyErr_SetString(PyExc_ImportError, "cannot import extension");
       return 0;
     }

     if (import_bob_io_base() < 0) {
       PyErr_Print();
       PyErr_SetString(PyExc_ImportError, "cannot import extension");
       return 0;
     }

     if (import_bob_sp() < 0) {
       PyErr_Print();
       PyErr_SetString(PyExc_ImportError, "cannot import extension");
       return 0;
     }

     if (import_bob_ip_gabor() < 0) {
       PyErr_Print();
       PyErr_SetString(PyExc_ImportError, "cannot import extension");
       return 0;
     }

     ...

   }




For each of the five classes above, there exist three objects that can be used in the bindings.
These objects are defined in the ``api.h`` header file:

Gabor wavelet
+++++++++++++

.. c:type:: PyBobIpGaborWaveletObject

   .. function:: boost::shared_ptr<bob::ip::gabor::Wavelet> cxx

      The shared pointer to object of the underlying `bob::ip::gabor::Wavelet` class.

.. c:var:: PyTypeObject PyBobIpGaborWavelet_Type

   The :c:type:`PyTypeObject` that defines the `bob::ip::gabor::Wavelet` class.

.. c:function:: int PyBobIpGaborWavelet_Check(PyObject* o)

   The function to check if the given :c:type:`PyObject` is castable to a :c:type:`PyBobIpGaborWaveletObject`.
   It returns ``1`` if it is, and ``0`` otherwise.


Gabor wavelet family
++++++++++++++++++++

.. c:type:: PyBobIpGaborTransformObject

   .. function:: boost::shared_ptr<bob::ip::gabor::Transform> cxx

      The shared pointer to object of the underlying `bob::ip::gabor::Transform` class.

.. c:var:: PyTypeObject PyBobIpGaborTransform_Type

   The :c:type:`PyTypeObject` that defines the `bob::ip::gabor::Transform` class.

.. c:function:: int PyBobIpGaborTransform_Check(PyObject* o)

   The function to check if the given :c:type:`PyObject` is castable to a :c:type:`PyBobIpGaborTransformObject`.
   It returns ``1`` if it is, and ``0`` otherwise.


Gabor jet
+++++++++

.. c:type:: PyBobIpGaborJetObject

   .. function:: boost::shared_ptr<bob::ip::gabor::Jet> cxx

      The shared pointer to object of the underlying `bob::ip::gabor::Jet` class.

.. c:var:: PyTypeObject PyBobIpGaborJet_Type

   The :c:type:`PyTypeObject` that defines the `bob::ip::gabor::Jet` class.

.. c:function:: int PyBobIpGaborJet_Check(PyObject* o)

   The function to check if the given :c:type:`PyObject` is castable to a :c:type:`PyBobIpGaborJetObject`.
   It returns ``1`` if it is, and ``0`` otherwise.


Gabor jet similarity
++++++++++++++++++++

.. c:type:: PyBobIpGaborSimilarityObject

   .. function:: boost::shared_ptr<bob::ip::gabor::Similarity> cxx

      The shared pointer to object of the underlying `bob::ip::gabor::Similarity` class.

.. c:var:: PyTypeObject PyBobIpGaborSimilarity_Type

   The :c:type:`PyTypeObject` that defines the `bob::ip::gabor::Similarity` class.

.. c:function:: int PyBobIpGaborSimilarity_Check(PyObject* o)

   The function to check if the given :c:type:`PyObject` is castable to a :c:type:`PyBobIpGaborSimilarityObject`.
   It returns ``1`` if it is, and ``0`` otherwise.


Gabor graph
+++++++++++

.. c:type:: PyBobIpGaborGraphObject

   .. function:: boost::shared_ptr<bob::ip::gabor::Graph> cxx

      The shared pointer to object of the underlying `bob::ip::gabor::Graph` class.

.. c:var:: PyTypeObject PyBobIpGaborGraph_Type

   The :c:type:`PyTypeObject` that defines the `bob::ip::gabor::Graph` class.

.. c:function:: int PyBobIpGaborGraph_Check(PyObject* o)

   The function to check if the given :c:type:`PyObject` is castable to a :c:type:`PyBobIpGaborGraphObject`.
   It returns ``1`` if it is, and ``0`` otherwise.



