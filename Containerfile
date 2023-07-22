FROM alpine:edge
RUN apk add \
	g++ \
	vim \
	make
ENV ENV=/etc/profile
RUN echo 'alias dir="ls -F"' >> /etc/profile.d/dir.sh
