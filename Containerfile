FROM alpine:edge
RUN apk add \
	g++ \
	vim \
	make \
	valgrind
ENV ENV=/etc/profile
RUN echo 'alias dir="ls -F"' >> /etc/profile.d/dir.sh
COPY cpp.vim /root/.vim/after/ftplugin/cpp.vim
WORKDIR /code
