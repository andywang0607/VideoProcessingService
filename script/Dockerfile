From ubuntu:20.04
MAINTAINER andywang0607

WORKDIR /workspace

# copy required file
COPY lib/ lib
COPY VideoProcessingService .

# set environment variable
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/workspace/lib
RUN echo $LD_LIBRARY_PATH

RUN ls /workspace

ENTRYPOINT ["./VideoProcessingService"]