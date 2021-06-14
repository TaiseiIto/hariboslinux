FROM ubuntu
RUN apt update
RUN apt install -y build-essential
RUN apt install -y git
WORKDIR ~
RUN git clone https://github.com/TaiseiIto/hariboslinux.git
WORKDIR hariboslinux
RUN make
CMD cd ~/hariboslinux

