-include ef.gy/base.mk include/ef.gy/base.mk

NAME:=prometheus

.third-party/libefgy/include/ef.gy/base.mk:
	mkdir .third-party || true
	cd .third-party && git clone git://github.com/ef-gy/libefgy.git

include/ef.gy/base.mk: .third-party/libefgy/include/ef.gy/base.mk
	ln -sfn ../.third-party/libefgy/include/ef.gy include/ef.gy

server src/server.cpp: include/asio.hpp
