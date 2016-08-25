FROM ubuntu
MAINTAINER Hans Simillion <hsimilli@vub.ac.be>

LABEL Description="Installs coolfluid dependencies on ubuntu"

########################################
# Download and install dependencies
RUN apt-get update && apt-get install -y \
	git \
	cmake \
	gcc \
	g++ \
	gfortran \
	libboost-all-dev \
	libeigen3-dev \
	liblapack-dev \
	liblas-dev \
	curl 

# Install python
RUN apt-get install -y \
	python \
	python-dev \
	python-pip \
	python-numpy \
	python-scipy \
	python-matplotlib

########################################
# Environment variables
# Define and create homefolder
ENV 	CF3 	/coolfluid 
ENV	PLU 	$CF3/plugins 
ENV  BUILD_DIR 	$CF3/build

RUN 	mkdir 	$CF3 \
		$PLU \
		$CF3/deps \
		$BUILD_DIR

########################################
# Download DLIB
RUN cd $CF3/deps && curl -L \
         https://github.com/davisking/dlib/archive/v19.0.tar.gz \
         -o dlib.tar.bz2 && \
         tar xf dlib.tar.bz2

# Download coolfluid
RUN git clone https://github.com/hsimilli/coolfluid3.git $CF3/kernel

# Download coolfluid-lss
RUN cd $PLU && git clone https://github.com/coolfluid/lss.git

########################################
# Run Cmake to configure coolfluid build

RUN 	cd $BUILD_DIR && rm -f CMakeCache.txt && \
	cmake                              \
		-DDLIB_HOME=$CF3/deps/dlib-19.0  \
  		-DCMAKE_BUILD_TYPE=Release       \
  		-DCF3_ENABLE_GUI=OFF             \
  		-DCF3_ENABLE_PROTO=OFF           \
  		-DCF3_PLUGIN_DIRS=$PLU           \
  		-DCF3_PLUGIN_LSS=ON              \
  		-DCF3_PLUGIN_LSS_PARDISO=ON      \
  		-DCF3_PLUGIN_LSS_MKL=ON          \
  		-DCF3_PLUGIN_UFEM=OFF            \
  		-DCF3_PLUGIN_UI=OFF              \
  		-DCF3_SKIP_CGNS=ON               \
  		-DCF3_SKIP_SUPERLU=ON            \
  		-DCF3_SKIP_TRILINOS=ON           \
	$CF3/kernel


########################################
# Build coolfluid 
# - with j8 to activate up to 8 cores
RUN	cd $BUILD_DIR && make -j8